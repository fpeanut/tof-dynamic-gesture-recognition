#pragma once

#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "DynamicModuleLoader.h"

using namespace DynamicModuleLoaderSpace;

using namespace std;
using namespace cv;


class C_IrisTracking
{
public:
	//_dll_name,_pbtxt_name���Բ���Ĭ�ϵ����֣�Ҳ�������Զ�������֣���Ҫ��ʵ�����ֶ�Ӧ
	C_IrisTracking(std::string _model_path, std::string _dll_name = "iris_tracking_dll.dll", std::string _pbtxt_name = "iris_tracking_cpu.pbtxt");
	~C_IrisTracking();
	bool m_Init_IrisTracking_OK = false;//��ʼ���ɹ���ʶ��
	bool InitIrisTracking();//��ʼ��IrisTracking
	std::vector<Point> landmarks;
	bool getLandmarks(Mat img);

private:
	std::string model_path;//dll��pbtxt�ļ���·��
	std::string dll_name;//ģ������
	std::string pbtxt_name;//pbtxt����

	typedef int(*FunC_IrisTracking_Iris_Tracking_Init)(const char* model_path);//��ʼ��ģ�ͺ������ļ�
	typedef float* (*FunC_IrisTracking_Iris_Tracking_Detect_Frame_New)(int image_width, int image_height, int image_channels, void* image_data, float *landmarks, int &size_, int bool_flag);//��ȡlandmarks
	typedef int(*FunC_IrisTracking_Iris_Tracking_Release)();//�ͷ���Դ

	FunC_IrisTracking_Iris_Tracking_Init IrisTracking_Iris_Tracking_Init = nullptr;
	FunC_IrisTracking_Iris_Tracking_Release IrisTracking_Iris_Tracking_Release = nullptr;
	FunC_IrisTracking_Iris_Tracking_Detect_Frame_New IrisTracking_Iris_Tracking_Detect_Frame_New = nullptr;

	DynamicModuleLoader dllLoader;

};