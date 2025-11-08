// main.cpp
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ===== TFLite headers (adjust include paths if needed) =====
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

// ---------- config: adapt to your model ----------
static const int INPUT_W = 416;    // Tiny-YOLOv4 commonly 416x416
static const int INPUT_H = 416;
static const int NUM_CLASSES = 80; // COCO
static const float CONF_THRESH = 0.34f;
static const float NMS_IOU_THRESH = 0.45f;
// anchors (COCO) for Tiny-YOLOv4 (scale 13 first, then 26)
static const float ANCHORS[6][2] = {
    {81,82}, {135,169}, {344,319}, // 13x13
    {10,14}, {23,27},  {37,58}     // 26x26
};

// -----------------------------------------------------------

struct Detection {
    float x1, y1, x2, y2;  // pixel coords in ORIGINAL image space
    int   class_id;
    float score;
    bool  suppressed=false;
};

// simple helpers
static inline float sigmoid(float x){ return 1.f / (1.f + std::exp(-x)); }

static size_t GetTensorElementCount(const TfLiteTensor* t) {
    size_t n = 1;
    for (int i = 0; i < t->dims->size; ++i)
        n *= t->dims->data[i];
    return n;
}

static float IoU(const Detection& a, const Detection& b) {
    float xx1 = std::max(a.x1, b.x1);
    float yy1 = std::max(a.y1, b.y1);
    float xx2 = std::min(a.x2, b.x2);
    float yy2 = std::min(a.y2, b.y2);
    float w = std::max(0.f, xx2 - xx1);
    float h = std::max(0.f, yy2 - yy1);
    float inter = w * h;
    float areaA = (a.x2 - a.x1) * (a.y2 - a.y1);
    float areaB = (b.x2 - b.x1) * (b.y2 - b.y1);
    return inter / std::max(1e-6f, (areaA + areaB - inter));
}

// Very small bilinear resize for RGB uint8 -> float (0..1)
static void resize_bilinear_rgb_u8_to_float(
    const unsigned char* src, int sw, int sh, int sc, // sc must be 3
    float* dst, int dw, int dh)
{
    const float x_scale = (float)sw / dw;
    const float y_scale = (float)sh / dh;
    for (int y = 0; y < dh; ++y) {
        float sy = (y + 0.5f) * y_scale - 0.5f;
        int y0 = (int)floorf(sy);
        int y1 = std::min(y0 + 1, sh - 1);
        float wy = sy - y0;
        y0 = std::max(0, y0);

        for (int x = 0; x < dw; ++x) {
            float sx = (x + 0.5f) * x_scale - 0.5f;
            int x0 = (int)floorf(sx);
            int x1 = std::min(x0 + 1, sw - 1);
            float wx = sx - x0;
            x0 = std::max(0, x0);

            const unsigned char* p00 = src + (y0 * sw + x0) * sc;
            const unsigned char* p01 = src + (y0 * sw + x1) * sc;
            const unsigned char* p10 = src + (y1 * sw + x0) * sc;
            const unsigned char* p11 = src + (y1 * sw + x1) * sc;

            for (int c = 0; c < 3; ++c) {
                float v = (1 - wy) * ((1 - wx) * p00[c] + wx * p01[c])
                        +    wy  * ((1 - wx) * p10[c] + wx * p11[c]);
                dst[(y * dw + x) * 3 + c] = v / 255.0f; // normalize 0..1
            }
        }
    }
}

// Optionally handle quantized input (uint8)
static void copy_to_quant_input(
    const unsigned char* src_rgb_u8, int sw, int sh, int sc,
    uint8_t* dst, int dw, int dh,
    int32_t zero_point, float scale)
{
    std::vector<float> tmp(dw * dh * 3);
    resize_bilinear_rgb_u8_to_float(src_rgb_u8, sw, sh, sc, tmp.data(), dw, dh);
    for (size_t i = 0; i < tmp.size(); ++i) {
        int32_t q = (int32_t)std::round(tmp[i] / scale) + zero_point;
        q = std::max(0, std::min(255, q));
        dst[i] = static_cast<uint8_t>(q);
    }
}

// Float input path
static void copy_to_float_input(
    const unsigned char* src_rgb_u8, int sw, int sh, int sc,
    float* dst, int dw, int dh)
{
    resize_bilinear_rgb_u8_to_float(src_rgb_u8, sw, sh, sc, dst, dw, dh);
}

// Simplified unified decode function that handles both layouts
static void decode_yolo_output(const float* out, int grid, int input_w, int input_h,
                                int orig_w, int orig_h, int anchor_offset, 
                                bool is_flattened, std::vector<Detection>& dets)
{
    const int stride = (5 + NUM_CLASSES);
    float scale_x = (float)orig_w;
    float scale_y = (float)orig_h;

    for (int gy = 0; gy < grid; ++gy) {
        for (int gx = 0; gx < grid; ++gx) {
            for (int a = 0; a < 3; ++a) {
                // Calculate index based on layout
                int idx;
                if (is_flattened) {
                    // Flattened: [grid, grid, 3*stride]
                    idx = (gy * grid + gx) * (3 * stride) + a * stride;
                } else {
                    // Separated: [grid, grid, 3, stride]
                    idx = ((gy * grid + gx) * 3 + a) * stride;
                }

                float objectness = sigmoid(out[idx + 4]);
                if (objectness < CONF_THRESH) continue;

                // Decode box coordinates
                float cx = (sigmoid(out[idx + 0]) + gx) / grid;
                float cy = (sigmoid(out[idx + 1]) + gy) / grid;
                float pw = ANCHORS[anchor_offset + a][0];
                float ph = ANCHORS[anchor_offset + a][1];
                float bw = (pw * std::exp(out[idx + 2])) / input_w;
                float bh = (ph * std::exp(out[idx + 3])) / input_h;

                // Find best class
                int best_c = -1;
                float best_p = 0.f;
                for (int c = 0; c < NUM_CLASSES; ++c) {
                    float pc = sigmoid(out[idx + 5 + c]);
                    if (pc > best_p) { best_p = pc; best_c = c; }
                }
                
                float score = objectness * best_p;
                if (score < CONF_THRESH) continue;

                // Convert to pixel coordinates
                float x = cx * scale_x;
                float y = cy * scale_y;
                float w = bw * scale_x;
                float h = bh * scale_y;

                Detection d;
                d.x1 = std::max(0.f, x - w * 0.5f);
                d.y1 = std::max(0.f, y - h * 0.5f);
                d.x2 = std::min((float)orig_w - 1, x + w * 0.5f);
                d.y2 = std::min((float)orig_h - 1, y + h * 0.5f);
                d.class_id = best_c;
                d.score = score;
                dets.push_back(d);
            }
        }
    }
}

// simple class-agnostic NMS
static void nms(std::vector<Detection>& dets, float iou_thresh) {
    std::sort(dets.begin(), dets.end(),
              [](const Detection& a, const Detection& b){return a.score > b.score;});
    for (size_t i = 0; i < dets.size(); ++i) {
        if (dets[i].suppressed) continue;
        for (size_t j = i + 1; j < dets.size(); ++j) {
            if (dets[j].suppressed) continue;
            if (IoU(dets[i], dets[j]) > iou_thresh)
                dets[j].suppressed = true;
        }
    }
}

// Read entire output tensor as float*, dequantizing if needed.
// Returns a vector<float> owning the data.
static std::vector<float> get_output_as_floats(const TfLiteTensor* t) {
    std::vector<float> out;
    if (t->type == kTfLiteFloat32) {
        const float* p = reinterpret_cast<const float*>(t->data.raw);
        out.assign(p, p + GetTensorElementCount(t));
    } else if (t->type == kTfLiteUInt8) {
        float scale = t->params.scale;
        int32_t zp = t->params.zero_point;
        const uint8_t* p = reinterpret_cast<const uint8_t*>(t->data.raw);
        out.resize(GetTensorElementCount(t));
        for (size_t i = 0; i < out.size(); ++i) out[i] = (p[i] - zp) * scale;
    } else {
        fprintf(stderr, "Unsupported output tensor type\n");
    }
    return out;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::printf("Usage: %s model.tflite image.jpg\n", argv[0]);
        return 1;
    }
    const std::string model_path = argv[1];
    const std::string image_path = argv[2];

    // Load image (force 3 channels RGB)
    int iw, ih, ic;
    unsigned char* img = stbi_load(image_path.c_str(), &iw, &ih, &ic, 3);
    if (!img) { std::cerr << "Failed to load image\n"; return 1; }
    ic = 3;

    // Load TFLite model
    auto model = tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
    if (!model) { std::cerr << "Failed to load model\n"; stbi_image_free(img); return 1; }

    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    std::unique_ptr<tflite::Interpreter> interpreter;
    builder(&interpreter);
    if (!interpreter) { std::cerr << "Failed to build interpreter\n"; stbi_image_free(img); return 1; }

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        std::cerr << "AllocateTensors failed\n"; stbi_image_free(img); return 1;
    }

    // Assume single input [1, H, W, 3]
    int input_idx = interpreter->inputs()[0];
    TfLiteTensor* input = interpreter->tensor(input_idx);
    if (input->dims->size != 4 || input->dims->data[1] != INPUT_H || input->dims->data[2] != INPUT_W || input->dims->data[3] != 3) {
        std::cerr << "Unexpected input shape\n";
        stbi_image_free(img); return 1;
    }

    // Fill input - TensorFlow Lite handles quantization automatically
    if (input->type == kTfLiteFloat32) {
        float* in = interpreter->typed_tensor<float>(input_idx);
        copy_to_float_input(img, iw, ih, ic, in, INPUT_W, INPUT_H);
    } else if (input->type == kTfLiteUInt8) {
        // For quantized models, still input as float and let TFLite handle conversion
        uint8_t* in = interpreter->typed_tensor<uint8_t>(input_idx);
        // Simple direct copy with normalization
        std::vector<float> tmp(INPUT_W * INPUT_H * 3);
        copy_to_float_input(img, iw, ih, ic, tmp.data(), INPUT_W, INPUT_H);
        // Convert to uint8 directly (0-255 range)
        for (size_t i = 0; i < tmp.size(); ++i) {
            in[i] = static_cast<uint8_t>(tmp[i] * 255.0f);
        }
    } else {
        std::cerr << "Unsupported input type\n";
        stbi_image_free(img); return 1;
    }

    // Inference with timing
    std::printf("\n🚀 Running inference...\n");
    auto start = std::chrono::high_resolution_clock::now();
    
    if (interpreter->Invoke() != kTfLiteOk) {
        std::cerr << "Invoke failed\n"; stbi_image_free(img); return 1;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    float fps = 1000.0f / duration_ms.count();
    
    std::printf("✅ Inference completed\n");
    std::printf("⏱️  Inference time: %ld ms (%.2f FPS)\n", duration_ms.count(), fps);

    // Expect 2 outputs: [1,13,13,255] and [1,26,26,255] for 80 classes
    // Output is already dequantized by TensorFlow Lite
    const TfLiteTensor* out0 = interpreter->output_tensor(0);
    const TfLiteTensor* out1 = interpreter->output_tensor(1);
    
    // Get outputs as floats - TFLite already handles dequantization
    std::vector<float> o0, o1;
    if (out0->type == kTfLiteFloat32) {
        const float* p = reinterpret_cast<const float*>(out0->data.raw);
        o0.assign(p, p + GetTensorElementCount(out0));
    } else {
        o0 = get_output_as_floats(out0);
    }
    
    if (out1->type == kTfLiteFloat32) {
        const float* p = reinterpret_cast<const float*>(out1->data.raw);
        o1.assign(p, p + GetTensorElementCount(out1));
    } else {
        o1 = get_output_as_floats(out1);
    }
    
    std::printf("Output 0 size: %zu, Output 1 size: %zu\n", o0.size(), o1.size());

    // Decode
    std::vector<Detection> dets;
    
    std::printf("\n📦 Decoding outputs...\n");
    std::printf("Output 0: [1, 26, 26, 255] - Decoding 26x26 grid (flattened)\n");
    std::printf("Output 1: [1, 13, 13, 3, 85] - Decoding 13x13 grid (separated)\n");
    
    decode_yolo_output(o0.data(), 26, INPUT_W, INPUT_H, iw, ih, 3, true, dets);
    decode_yolo_output(o1.data(), 13, INPUT_W, INPUT_H, iw, ih, 0, false, dets);
    
    std::printf("Total detections before NMS: %zu\n", dets.size());

    // NMS
    nms(dets, NMS_IOU_THRESH);
    
    // Count final detections
    int final_count = 0;
    for (const auto& d : dets) {
        if (!d.suppressed && d.score >= CONF_THRESH) final_count++;
    }
    std::printf("Detections after NMS: %d\n", final_count);

    // Print results
    std::printf("\n🎯 Detection Results:\n");
    std::printf("%-8s %-12s %-40s\n", "Class", "Confidence", "Bounding Box [x1, y1, x2, y2]");
    std::printf("%s\n", std::string(70, '-').c_str());
    
    for (const auto& d : dets) {
        if (d.suppressed) continue;
        if (d.score < CONF_THRESH) continue;
        std::printf("%-8d %.2f%%       [%4d, %4d, %4d, %4d]\n",
            d.class_id, d.score * 100.0f,
            (int)d.x1, (int)d.y1, (int)d.x2, (int)d.y2);
    }
    
    // Summary
    std::cout << std::string(70, '=') << std::endl;
    std::printf("📊 Summary:\n");
    std::printf("  Image: %s (%dx%d)\n", image_path.c_str(), iw, ih);
    std::printf("  Detections: %d objects found\n", final_count);
    std::printf("  Inference time: %ld ms (%.2f FPS)\n", duration_ms.count(), fps);
    std::printf("  Confidence threshold: %.2f\n", CONF_THRESH);
    std::printf("  NMS IoU threshold: %.2f\n", NMS_IOU_THRESH);
    std::cout << std::string(70, '=') << std::endl;

    stbi_image_free(img);
    return 0;
}
