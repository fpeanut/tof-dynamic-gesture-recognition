#pragma once//用于防止头文件重定义
#include <iostream>
#include <assert.h>
#include <vector>
#include <onnxruntime_cxx_api.h>
using namespace std;
using namespace Ort;
class loadonnx
{
public:
	loadonnx(const wchar_t* path);
	void init();
	void Reconginition(vector<float>input);
	void log_print();
private:
	const wchar_t* model_path;
	Ort::Env env= Env(ORT_LOGGING_LEVEL_VERBOSE, "LSTM");	//设置为VERBOSE，方便控制台输出时看到是使用了cpu还是gpu执行
	Ort::SessionOptions session_options;
	Ort::Session *session;
	std::vector<const char*> input_names;
	std::vector<const char*> output_names;
	vector<int64_t> input_dims;
	vector<int64_t> output_dims;
};

