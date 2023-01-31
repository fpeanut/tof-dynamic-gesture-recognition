#include "loadonnx.h"

loadonnx::loadonnx(const wchar_t* path) {
	this->model_path = path;
	init();
}

void loadonnx::init() {
	//1.创建onnx session
	// 使用五个线程执行op,提升速度
	session_options.SetIntraOpNumThreads(1);
	// 第二个参数代表GPU device_id = 0，注释这行就是cpu执行
	//OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 0);
	// ORT_ENABLE_ALL: To Enable All possible opitmizations
	session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
	//2.加载模型
	//const wchar_t* model_path = L"LSTM_epcoh800.onnx";//wchar_t 宽字节16位 32位，加L，const保护指针不被修改
	session=new Ort::Session(env, model_path, session_options);
	Ort::AllocatorWithDefaultOptions allocator;
	//3.获取模型信息
	const char* input_name = session->GetInputName(0, allocator);
	const char* output_name = session->GetOutputName(0, allocator);//输出与输入名字
	input_dims = session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
	output_dims = session->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();//获取输入输出的维度信息
	input_dims[0] = output_dims[0] = 1;//将输入输出维度设为1
	input_names={ input_name };
	output_names={ output_name };
	
}

void loadonnx::Reconginition(vector<float>input) {
	//4.准备输入
	//创建input tensor
	auto memory_info = Ort::MemoryInfo::CreateCpu(
		OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
	Value input_tensor_ = Value::CreateTensor<float>(memory_info, input.data(), input.size(), input_dims.data(), input_dims.size());
	//5.开始推理	
	auto outputValues = session->Run(
		Ort::RunOptions{ nullptr }, // e.g. set a verbosity level only for this run
		input_names.data(), &input_tensor_, 1, // input to set
		output_names.data(), output_names.size());
	//6.获取输出
	float* output0 = outputValues[0].GetTensorMutableData<float>();//定义数组获取值

	//7.获取最大值索引
	float max_pre = 0;
	int max_index = 0;
	for (int i = 0; i < 12; i++) {
		if (max_pre < output0[i]) {
			max_pre = output0[i];
			max_index = i + 1;
		}
		cout << output0[i] << " ";
	}
	cout << endl;
	cout << "预测分类：" << max_index << endl;
}

void loadonnx::log_print() {
	//其他输出
	cout << "输入输出名字：" << input_names[0] << "," << output_names[0] << endl;
	cout << "输入维度：";
	for (int i = 0; i < input_dims.size(); i++) {
		cout << input_dims[i] << ",";
	}
	cout << endl;
	cout << "输出维度：";
	for (int i = 0; i < output_dims.size(); i++) {
		cout << output_dims[i] << ",";
	}
	cout << endl;
}
