#include <iostream>
#include "Tflite_LSTM.h"

int main()
{
	vector<vector<float>> inputpoints;
	for (int i = 0; i < 30; i++) {
		vector<float> temp;
		for (int j = 0; j < 63; j++) {
			temp.push_back(0.5);
		}
		inputpoints.push_back(temp);
	}


	Tflite_LSTM *tflstm = nullptr;
	tflstm = new Tflite_LSTM("../model/converted_model.tflite", 1, 30, 63, 0.8);
	std::pair<string, float> test;
	test = tflstm->DetectRestult(inputpoints);
	std::cout << test.first << "," << test.second << endl;

	system("pause");
}

