#include "Tflite_LSTM.h"

Tflite_LSTM::Tflite_LSTM(std::string _model_path, int threadnumber_, int sques_, int features_, float threshold_) {
	this->modelpath = _model_path;
	this->threadnumber = threadnumber_;
	this->sques = sques_;
	this->features = features_;
	this->threshold = threshold_;
	if (LoadTfliteModel()) {
		std::cout << "model load successfully!" << endl;
	}
}

Tflite_LSTM::~Tflite_LSTM() {

}
//初始化lstm
bool Tflite_LSTM::LoadTfliteModel() {
	//初始化tflit，读取模型参数
	// 1，创建模型和解释器对象，并加载模型
	model = tflite::FlatBufferModel::BuildFromFile(modelpath.c_str());
	if (!model)
	{
		std::cout << "\nFailed to load the model.\n"
			<< std::endl;
		return false;
	}
	// 2，将模型中的tensor映射写入到解释器对象中
	tflite::InterpreterBuilder(*model, resolver)(&interpreter_);
	TfLiteStatus status = interpreter_->AllocateTensors();
	interpreter_->SetNumThreads(threadnumber);
	if (status != kTfLiteOk)
	{
		std::cout << "\nFailed to allocate the memory for tensors.\n"
			<< std::endl;
		return false;
	}
	//3.打印网络结构 
	input_tensor = interpreter_->tensor(interpreter_->inputs()[0]);
	TfLiteIntArray *dims = input_tensor->dims;
	int _in_samples = dims->data[0];
	int _in_sqes = dims->data[1];
	int _in_features = dims->data[2];
	int _in_channels = dims->data[3];
	int _in_type = input_tensor->type;
	std::cout << _in_samples << "," << _in_sqes << "," << _in_features << "," << _in_channels << "," << _in_type << std::endl;
	return true;
	
}
//模型预估
std::pair<string, float> Tflite_LSTM::DetectRestult(std::vector<std::vector<float>> LSTMData) {
	std::pair<string, float> result_lstm;
	if (LSTMData.size() != sques) {
		std::cout << "time squence scals is error!" << std::endl;
		exit(1);
	}
	//4.定义输入
	int input = interpreter_->inputs()[0];
	float* input_data_ptr = interpreter_->typed_tensor<float>(input);
	for (int i = 0; i < sques; ++i) {
		for (int j = 0; j < features; j++) {
			*(input_data_ptr) = (float)LSTMData[i][j];
			input_data_ptr++;
		}
	}
	//5.推理
	interpreter_->Invoke();
	//6. 得出结果
	int output = interpreter_->outputs()[0];
	TfLiteIntArray *output_dims = interpreter_->tensor(output)->dims;
	auto output_size = output_dims->data[output_dims->size - 1];
	std::vector<std::pair<float, int>> top_results;

	switch (interpreter_->tensor(output)->type)
	{
	case kTfLiteFloat32:
		tflite::label_image::get_top_n<float>(interpreter_->typed_output_tensor<float>(0), output_size, 1, threshold, &top_results, kTfLiteFloat32);
		break;
	case kTfLiteUInt8:
		tflite::label_image::get_top_n<uint8_t>(interpreter_->typed_output_tensor<uint8_t>(0), output_size, 1, threshold, &top_results, kTfLiteUInt8);
		break;
	default:
		fprintf(stderr, "cannot handle output type\n");
		exit(-1);
	}
	// Print labels with confidence in input image
	for (const auto &result : top_results)
	{
		float scores = result.first;
		int labels = result.second;
		//std::cout << "labels: " << labels << ", " << "scores:" << scores << std::endl;
		if (scores > 0.8) {
			switch (labels)
			{
			case 0:result_lstm = make_pair("HandFlickDown", scores);
				break;
			case 1:result_lstm = make_pair("HandLeft", scores);
				break;
			case 2:result_lstm = make_pair("HandRight", scores);
				break;
			case 3:result_lstm = make_pair("HandFlickUp", scores);
				break;
			default:
				break;
			}
		}
		else
		{
			result_lstm = make_pair("None", scores);
		}

	}
	return result_lstm;

}
