#include"loadonnx.h"

int main() {
	vector<float>input;
	for (int i = 0; i < 1890; i++) {
		input.push_back(1);
	}
	loadonnx *newort = nullptr;
	newort = new loadonnx(L"LSTM_best.onnx");
	for(int i=0;i<100;i++)
		newort->Reconginition(input);
}