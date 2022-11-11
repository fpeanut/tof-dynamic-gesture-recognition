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


class C_PoseTracking
{
public:
	//_dll_name,_pbtxt_name���Բ���Ĭ�ϵ����֣�Ҳ�������Զ�������֣���Ҫ��ʵ�����ֶ�Ӧ
	C_PoseTracking(std::string _model_path, std::string _dll_name = "pose_tracking_dll.dll", std::string _pbtxt_name = "pose_tracking_cpu.pbtxt");
	~C_PoseTracking();
	bool m_Init_PoseTracking_OK = false;//��ʼ���ɹ���ʶ��
	bool InitPoseTracking();//��ʼ��facemesh
	std::vector<Point> landmarks;
	bool getLandmarks(Mat img, bool multi_objects = false);//multi_faces==false�������1���ֵ�����,Ϊtrueʱ����ʵ�ʼ�⵽��������

private:
	std::string model_path;//dll��pbtxt�ļ���·��
	std::string dll_name;//ģ������
	std::string pbtxt_name;//pbtxt����

	typedef int(*FunC_PoseTracking_Pose_Tracking_Init)(const char* model_path);//��ʼ��ģ�ͺ������ļ�
	typedef float* (*FunC_PoseTracking_Pose_Tracking_Detect_Frame_New)(int image_width, int image_height, int image_channels, void* image_data, float *landmarks, int &size_, int bool_flag);//��ȡlandmarks
	typedef int(*FunC_PoseTracking_Pose_Tracking_Release)();//�ͷ���Դ

	FunC_PoseTracking_Pose_Tracking_Init PoseTracking_Pose_Tracking_Init = nullptr;
	FunC_PoseTracking_Pose_Tracking_Release PoseTracking_Pose_Tracking_Release = nullptr;
	FunC_PoseTracking_Pose_Tracking_Detect_Frame_New PoseTracking_Pose_Tracking_Detect_Frame_New = nullptr;

	DynamicModuleLoader dllLoader;

};