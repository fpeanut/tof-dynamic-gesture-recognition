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


class C_HandTracking
{
public:
	//_dll_name,_pbtxt_name���Բ���Ĭ�ϵ����֣�Ҳ�������Զ�������֣���Ҫ��ʵ�����ֶ�Ӧ
	C_HandTracking(std::string _model_path, std::string _dll_name = "hand_tracking_dll.dll", std::string _pbtxt_name = "hand_tracking_desktop_live.pbtxt");
	~C_HandTracking();
	bool m_Init_HandTracking_OK = false;//��ʼ���ɹ���ʶ��
	bool InitHandTracking();//��ʼ��facemesh
	std::vector<Point> landmarks;
	bool getLandmarks(Mat img, bool multi_objects = false);//multi_faces==false�������1���ֵ�����,Ϊtrueʱ����ʵ�ʼ�⵽��������

private:
	std::string model_path;//dll��pbtxt�ļ���·��
	std::string dll_name;//ģ������
	std::string pbtxt_name;//pbtxt����

	typedef int(*FunC_HandTracking_Hand_Tracking_Init)(const char* model_path);//��ʼ��ģ�ͺ������ļ�
	typedef float* (*FunC_HandTracking_Hand_Tracking_Detect_Frame_New)(int image_width, int image_height, int image_channels, void* image_data, float *landmarks, int &size_, int bool_flag);//��ȡlandmarks
	typedef int(*FunC_HandTracking_Hand_Tracking_Release)();//�ͷ���Դ

	FunC_HandTracking_Hand_Tracking_Init HandTracking_Hand_Tracking_Init = nullptr;
	FunC_HandTracking_Hand_Tracking_Release HandTracking_Hand_Tracking_Release = nullptr;
	FunC_HandTracking_Hand_Tracking_Detect_Frame_New HandTracking_Hand_Tracking_Detect_Frame_New = nullptr;

	DynamicModuleLoader dllLoader;


	//typedef void(*LandmarksCallBack)(float* landmarks, int& landmarks_size);	
	//typedef int(*FunC_HandTracking_Hand_Tracking_Reigeter_Landmarks_Callback)(LandmarksCallBack func);
	//typedef int(*FunC_HandTracking_Hand_Tracking_Detect_Frame)(void* image_data, int image_width, int image_height, int image_channels, bool log_flag);
	//FunC_HandTracking_Hand_Tracking_Reigeter_Landmarks_Callback HandTracking_Hand_Tracking_Reigeter_Landmarks_Callback = nullptr;
	//FunC_HandTracking_Hand_Tracking_Detect_Frame HandTracking_Hand_Tracking_Detect_Frame = nullptr;

};