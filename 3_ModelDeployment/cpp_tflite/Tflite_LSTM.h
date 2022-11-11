#pragma once
#include <iostream>
#include <vector>
#include<string>
//导入tflite头文件
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/tools/gen_op_registration.h"
#include "tensorflow/lite/examples/label_image/get_top_n.h"
using namespace std;
class Tflite_LSTM
{
public:
	Tflite_LSTM(std::string _model_path, int threadnumber_, int sques_, int features_, float threshold_);
	~Tflite_LSTM();

	std::unique_ptr<tflite::FlatBufferModel> model;//模型加载器
	tflite::ops::builtin::BuiltinOpResolver resolver;
	std::unique_ptr<tflite::Interpreter> interpreter_;//模型解释器
	TfLiteTensor* input_tensor = nullptr;
	TfLiteTensor* output_landmarks = nullptr;

	//初始化lstm
	bool LoadTfliteModel();
	//模型预估
	std::pair<string, float> DetectRestult(std::vector<std::vector<float>> LSTMData);

private:
	/*tflite模型参数*/
	string modelpath;//模型路径
	int threadnumber;//线程数
	int sques;
	int features;
	float threshold;

};

