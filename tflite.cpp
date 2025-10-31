#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

int main() {
    const char* model_path = "model.tflite";

    // Load model
    std::unique_ptr<tflite::FlatBufferModel> model =
        tflite::FlatBufferModel::BuildFromFile(model_path);
    if (!model) {
        std::cerr << "❌ Failed to load model: " << model_path << std::endl;
        return 1;
    }
    std::cout << "✅ Model loaded successfully\n";

    // Build the interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    std::unique_ptr<tflite::Interpreter> interpreter;

    if (builder(&interpreter) != kTfLiteOk) {
        std::cerr << "❌ Failed to build interpreter\n";
        return 1;
    }
    std::cout << "✅ Interpreter built successfully\n";

    // Allocate tensors
    if (interpreter->AllocateTensors() != kTfLiteOk) {
        std::cerr << "❌ Failed to allocate tensors\n";
        return 1;
    }
    std::cout << "✅ Tensors allocated successfully\n";

    // Get input tensor info
    int input_idx = interpreter->inputs()[0];
    TfLiteTensor* input_tensor = interpreter->tensor(input_idx);
    
    std::cout << "\n📊 Model Information:\n";
    std::cout << "Input tensor: " << input_tensor->name << std::endl;
    std::cout << "Input shape: [";
    for (int i = 0; i < input_tensor->dims->size; i++) {
        std::cout << input_tensor->dims->data[i];
        if (i < input_tensor->dims->size - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    std::cout << "Input type: " << TfLiteTypeGetName(input_tensor->type) << std::endl;
    
    // Get output tensor info
    int output_idx = interpreter->outputs()[0];
    TfLiteTensor* output_tensor = interpreter->tensor(output_idx);
    std::cout << "Output tensor: " << output_tensor->name << std::endl;
    std::cout << "Output shape: [";
    for (int i = 0; i < output_tensor->dims->size; i++) {
        std::cout << output_tensor->dims->data[i];
        if (i < output_tensor->dims->size - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    std::cout << "Output type: " << TfLiteTypeGetName(output_tensor->type) << std::endl;

    // Fill input tensor with dummy data (normalized image data between 0-1)
    std::cout << "\n🔄 Preparing input data...\n";
    int input_size = 1;
    for (int i = 0; i < input_tensor->dims->size; i++) {
        input_size *= input_tensor->dims->data[i];
    }
    
    // Fill with dummy normalized values (simulating a preprocessed image)
    float* input_data = interpreter->typed_input_tensor<float>(0);
    for (int i = 0; i < input_size; i++) {
        input_data[i] = 0.5f; // Neutral gray color
    }
    std::cout << "✅ Input data prepared (size: " << input_size << ")\n";

    // Run inference
    std::cout << "\n🚀 Running inference...\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    if (interpreter->Invoke() != kTfLiteOk) {
        std::cerr << "❌ Failed to invoke interpreter\n";
        return 1;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "✅ Inference completed in " << duration.count() << " ms\n";

    // Get output results
    float* output_data = interpreter->typed_output_tensor<float>(0);
    int output_size = 1;
    for (int i = 0; i < output_tensor->dims->size; i++) {
        output_size *= output_tensor->dims->data[i];
    }

    // Find top 5 predictions
    std::cout << "\n🎯 Top 5 Predictions:\n";
    std::vector<std::pair<int, float>> predictions;
    for (int i = 0; i < output_size; i++) {
        predictions.push_back({i, output_data[i]});
    }
    std::sort(predictions.begin(), predictions.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (int i = 0; i < std::min(5, output_size); i++) {
        std::cout << "  Class " << predictions[i].first 
                  << ": " << predictions[i].second * 100.0f << "%\n";
    }

    std::cout << "\n✅ MobileNetV1 inference successful!\n";
    return 0;
}
