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
	//_dll_name,_pbtxt_name可以采用默认的名字，也可以用自定义的名字，但要和实际名字对应
	C_FaceMesh(std::string _model_path,std::string _dll_name= "face_mesh_dll.dll",std::string _pbtxt_name= "face_mesh_desktop_live.pbtxt");
	~C_FaceMesh();
	bool m_Init_FaceMesh_OK = false;//初始化成功标识符
	bool InitFaceMesh();//初始化facemesh
	std::vector<Point>landmarks;
	bool getLandmarks(Mat img, bool multi_objects =false);//multi_faces==false返回最多1张人脸的数据,为true时返回实际检测到的人脸数据

private:
	std::string model_path;//dll和pbtxt文件的路径
	std::string dll_name;//模型名称
	std::string pbtxt_name;//pbtxt名称

	typedef int(*FunC_FaceMesh_Face_Mesh_Init)(const char* model_path);//初始化模型和配置文件
	typedef float* (*FunC_FaceMesh_Face_Mesh_Detect_Frame_New)(void* image_data, int image_width, int image_height, int image_channels, float *landmarks, int &size_, bool log_flag);//获取landmarks
	typedef int(*FunC_FaceMesh_Face_Mesh_Release)();//释放资源

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

