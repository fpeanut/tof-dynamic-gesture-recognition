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
	//_dll_name,_pbtxt_name可以采用默认的名字，也可以用自定义的名字，但要和实际名字对应
	C_HolisticTracking(std::string _model_path, std::string _dll_name = "holistic_tracking_dll.dll", std::string _pbtxt_name = "holistic_tracking_cpu.pbtxt");
	~C_HolisticTracking();
	bool m_Init_HolisticTracking_OK = false;//初始化成功标识符
	bool InitHolisticTracking();//初始化
	std::vector<Point> face_landmarks;
	std::vector<Point> pose_landmarks;
	std::vector<Point> left_landmarks;
	std::vector<Point> right_landmarks;
	bool getLandmarks(Mat img);

private:
	std::string model_path;//dll和pbtxt文件的路径
	std::string dll_name;//模型名称
	std::string pbtxt_name;//pbtxt名称

	typedef int(*FunC_MediapipeHolisticTrackingInit)(const char* model_path);//初始化模型和配置文件
	typedef float* (*FunC_HolisticTracking_Holistic_Tracking_Detect_Frame_New)(int image_width, int image_height, int image_channels, void* image_data, float *landmarks, 
		int& face_size, int & pose_size, int & left_size, int & right_size, bool log_flag);//获取landmarks
	typedef int(*FunC_HolisticTracking_Holistic_Tracking_Release)();//释放资源

	FunC_MediapipeHolisticTrackingInit HolisticTracking_Holistic_Tracking_Init = nullptr;
	FunC_HolisticTracking_Holistic_Tracking_Release HolisticTracking_Holistic_Tracking_Release = nullptr;
	FunC_HolisticTracking_Holistic_Tracking_Detect_Frame_New HolisticTracking_Holistic_Tracking_Detect_Frame_New = nullptr;

	DynamicModuleLoader dllLoader;



};