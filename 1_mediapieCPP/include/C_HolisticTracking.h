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


class C_HolisticTracking
{
public:
	//_dll_name,_pbtxt_name���Բ���Ĭ�ϵ����֣�Ҳ�������Զ�������֣���Ҫ��ʵ�����ֶ�Ӧ
	C_HolisticTracking(std::string _model_path, std::string _dll_name = "holistic_tracking_dll.dll", std::string _pbtxt_name = "holistic_tracking_cpu.pbtxt");
	~C_HolisticTracking();
	bool m_Init_HolisticTracking_OK = false;//��ʼ���ɹ���ʶ��
	bool InitHolisticTracking();//��ʼ��
	std::vector<Point> face_landmarks;
	std::vector<Point> pose_landmarks;
	std::vector<Point> left_landmarks;
	std::vector<Point> right_landmarks;
	bool getLandmarks(Mat img);

private:
	std::string model_path;//dll��pbtxt�ļ���·��
	std::string dll_name;//ģ������
	std::string pbtxt_name;//pbtxt����

	typedef int(*FunC_MediapipeHolisticTrackingInit)(const char* model_path);//��ʼ��ģ�ͺ������ļ�
	typedef float* (*FunC_HolisticTracking_Holistic_Tracking_Detect_Frame_New)(int image_width, int image_height, int image_channels, void* image_data, float *landmarks, 
		int& face_size, int & pose_size, int & left_size, int & right_size, bool log_flag);//��ȡlandmarks
	typedef int(*FunC_HolisticTracking_Holistic_Tracking_Release)();//�ͷ���Դ

	FunC_MediapipeHolisticTrackingInit HolisticTracking_Holistic_Tracking_Init = nullptr;
	FunC_HolisticTracking_Holistic_Tracking_Release HolisticTracking_Holistic_Tracking_Release = nullptr;
	FunC_HolisticTracking_Holistic_Tracking_Detect_Frame_New HolisticTracking_Holistic_Tracking_Detect_Frame_New = nullptr;

	DynamicModuleLoader dllLoader;



};