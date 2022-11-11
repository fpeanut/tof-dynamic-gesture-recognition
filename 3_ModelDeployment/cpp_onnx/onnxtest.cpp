#pragma once//���ڷ�ֹͷ�ļ��ض���
#include <iostream>
#include <assert.h>
#include <vector>
#include <onnxruntime_cxx_api.h>
using namespace std;
using namespace Ort;
#define isPrint

int main(int argc, char* argv[])
{
	vector<float>input;
	for (int i = 0; i < 1890; i++) {
		input.push_back(1);
	}
	//1.����onnx session
	//����ΪVERBOSE���������̨���ʱ������ʹ����cpu����gpuִ��
	Ort::Env env(ORT_LOGGING_LEVEL_VERBOSE, "test");
	Ort::SessionOptions session_options;
	// ʹ������߳�ִ��op,�����ٶ�
	session_options.SetIntraOpNumThreads(1);
	// �ڶ�����������GPU device_id = 0��ע�����о���cpuִ��
	//OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 0);
	// ORT_ENABLE_ALL: To Enable All possible opitmizations
	session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
	//2.����ģ��
	const wchar_t* model_path = L"LSTM_epcoh800.onnx";//wchar_t ���ֽ�16λ 32λ����L��const����ָ�벻���޸�
	Ort::Session session(env, model_path, session_options);
	Ort::AllocatorWithDefaultOptions allocator;
	//3.��ȡģ����Ϣ
	const char* input_name = session.GetInputName(0, allocator);
	const char* output_name = session.GetOutputName(0, allocator);//�������������
	auto input_dims = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
	auto output_dims = session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();//��ȡ���������ά����Ϣ
	input_dims[0] = output_dims[0] = 1;//���������ά����Ϊ1
	std::vector<const char*> input_names{ input_name };
	std::vector<const char*> output_names{ output_name };

	//4.׼������
	//����input tensor
	auto memory_info = Ort::MemoryInfo::CreateCpu(
		OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
	Value input_tensor_ = Value::CreateTensor<float>(memory_info, input.data(), input.size(), input_dims.data(), input_dims.size());
	//5.��ʼ����	
	auto outputValues = session.Run(
		Ort::RunOptions{ nullptr }, // e.g. set a verbosity level only for this run
		input_names.data(), &input_tensor_, 1, // input to set
		output_names.data(), output_names.size());
	//6.��ȡ���
	float* output0 = outputValues[0].GetTensorMutableData<float>();//���������ȡֵ
	//7.��ȡ���ֵ����
	float max_pre = 0;
	int max_index = 0;
	for (int i = 0; i < 12; i++) {
		if (max_pre < output0[i]) {
			max_pre = output0[i];
			max_index = i+1;
		}
		cout << output0[i]<<" ";
	}
	cout << endl;
	cout << "Ԥ����ࣺ" << max_index << endl;;
#ifdef isPrint
	//�������
	cout << "����������֣�" << input_name << "," << output_name << endl;
	cout << "����ά�ȣ�";
	for (int i = 0; i < input_dims.size(); i++) {
		cout << input_dims[i] << ",";
	}
	cout << endl;
	cout << "���ά�ȣ�";
	for (int i = 0; i < output_dims.size(); i++) {
		cout << output_dims[i] << ",";
	}
	cout << endl;
#else
	cout << "�����" << endl;
#endif // isPrint

}
