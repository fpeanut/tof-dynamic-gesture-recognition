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


class C_FaceMesh
{
public:
	//_dll_name,_pbtxt_name���Բ���Ĭ�ϵ����֣�Ҳ�������Զ�������֣���Ҫ��ʵ�����ֶ�Ӧ
	C_FaceMesh(std::string _model_path,std::string _dll_name= "face_mesh_dll.dll",std::string _pbtxt_name= "face_mesh_desktop_live.pbtxt");
	~C_FaceMesh();
	bool m_Init_FaceMesh_OK = false;//��ʼ���ɹ���ʶ��
	bool InitFaceMesh();//��ʼ��facemesh
	std::vector<Point>landmarks;
	bool getLandmarks(Mat img, bool multi_objects =false);//multi_faces==false�������1������������,Ϊtrueʱ����ʵ�ʼ�⵽����������

private:
	std::string model_path;//dll��pbtxt�ļ���·��
	std::string dll_name;//ģ������
	std::string pbtxt_name;//pbtxt����

	typedef int(*FunC_FaceMesh_Face_Mesh_Init)(const char* model_path);//��ʼ��ģ�ͺ������ļ�
	typedef float* (*FunC_FaceMesh_Face_Mesh_Detect_Frame_New)(void* image_data, int image_width, int image_height, int image_channels, float *landmarks, int &size_, bool log_flag);//��ȡlandmarks
	typedef int(*FunC_FaceMesh_Face_Mesh_Release)();//�ͷ���Դ

	FunC_FaceMesh_Face_Mesh_Init FaceMesh_Face_Mesh_Init = nullptr;
	FunC_FaceMesh_Face_Mesh_Release FaceMesh_Face_Mesh_Release = nullptr;
	FunC_FaceMesh_Face_Mesh_Detect_Frame_New FaceMesh_Face_Mesh_Detect_Frame_New = nullptr;

	DynamicModuleLoader dllLoader;


	//typedef void(*LandmarksCallBack)(float* landmarks, int& landmarks_size);	
	//typedef int(*FunC_FaceMesh_Face_Mesh_Reigeter_Landmarks_Callback)(LandmarksCallBack func);
	//typedef int(*FunC_FaceMesh_Face_Mesh_Detect_Frame)(void* image_data, int image_width, int image_height, int image_channels, bool log_flag);
	//FunC_FaceMesh_Face_Mesh_Reigeter_Landmarks_Callback FaceMesh_Face_Mesh_Reigeter_Landmarks_Callback = nullptr;
	//FunC_FaceMesh_Face_Mesh_Detect_Frame FaceMesh_Face_Mesh_Detect_Frame = nullptr;

};

