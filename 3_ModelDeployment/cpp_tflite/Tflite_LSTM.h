#pragma once
#include <iostream>
#include <vector>
#include<string>
//����tfliteͷ�ļ�
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

	std::unique_ptr<tflite::FlatBufferModel> model;//ģ�ͼ�����
	tflite::ops::builtin::BuiltinOpResolver resolver;
	std::unique_ptr<tflite::Interpreter> interpreter_;//ģ�ͽ�����
	TfLiteTensor* input_tensor = nullptr;
	TfLiteTensor* output_landmarks = nullptr;

	//��ʼ��lstm
	bool LoadTfliteModel();
	//ģ��Ԥ��
	std::pair<string, float> DetectRestult(std::vector<std::vector<float>> LSTMData);

private:
	/*tfliteģ�Ͳ���*/
	string modelpath;//ģ��·��
	int threadnumber;//�߳���
	int sques;
	int features;
	float threshold;

};

