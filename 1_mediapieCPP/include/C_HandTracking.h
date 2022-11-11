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
	//_dll_name,_pbtxt_name可以采用默认的名字，也可以用自定义的名字，但要和实际名字对应
	C_HandTracking(std::string _model_path, std::string _dll_name = "hand_tracking_dll.dll", std::string _pbtxt_name = "hand_tracking_desktop_live.pbtxt");
	~C_HandTracking();
	bool m_Init_HandTracking_OK = false;//初始化成功标识符
	bool InitHandTracking();//初始化facemesh
	std::vector<Point> landmarks;
	bool getLandmarks(Mat img, bool multi_objects = false);//multi_faces==false返回最多1个手的数据,为true时返回实际检测到的手数据

private:
	std::string model_path;//dll和pbtxt文件的路径
	std::string dll_name;//模型名称
	std::string pbtxt_name;//pbtxt名称

	typedef int(*FunC_HandTracking_Hand_Tracking_Init)(const char* model_path);//初始化模型和配置文件
	typedef float* (*FunC_HandTracking_Hand_Tracking_Detect_Frame_New)(int image_width, int image_height, int image_channels, void* image_data, float *landmarks, int &size_, int bool_flag);//获取landmarks
	typedef int(*FunC_HandTracking_Hand_Tracking_Release)();//释放资源

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