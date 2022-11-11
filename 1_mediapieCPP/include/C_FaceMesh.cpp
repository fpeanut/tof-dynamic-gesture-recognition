#include "C_FaceMesh.h"


C_FaceMesh::C_FaceMesh(std::string _model_path, std::string _dll_name, std::string _pbtxt_name)
{
	model_path = _model_path;
	dll_name = _dll_name;
	pbtxt_name = _pbtxt_name;
	if (InitFaceMesh())
	{
		m_Init_FaceMesh_OK = true;
		cout << "Init FaceMesh: True" << endl;
	}
	else
	{
		m_Init_FaceMesh_OK = false;
		cout << "Init FaceMesh: False" << endl;
	}
}

//��������
C_FaceMesh::~C_FaceMesh(){
	FaceMesh_Face_Mesh_Release();
}
bool C_FaceMesh::InitFaceMesh()
{
	if (dllLoader.IsFileExist(model_path+"\\"+dll_name))
	{
		std::cout << "dll����:" << model_path<<"\\"<< dll_name<<std::endl;

		if (dllLoader.LoadDynamicModule(model_path + "\\" + dll_name))
		{
			std::cout << "dll���سɹ���" << std::endl;

			// ��ȡFaceMesh_Face_Mesh_Init
			void* p_FaceMesh_Face_Mesh_Init = dllLoader.GetFunction("FaceMesh_Face_Mesh_Init");
			if (p_FaceMesh_Face_Mesh_Init != nullptr)
			{
				FaceMesh_Face_Mesh_Init = (FunC_FaceMesh_Face_Mesh_Init(p_FaceMesh_Face_Mesh_Init));
				if (FaceMesh_Face_Mesh_Init != nullptr)
				{
					std::cout << "[CallBack] FaceMesh_Face_Mesh_Init��ȡ�ɹ�" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] �޷���dll���ҵ�FaceMesh_Face_Mesh_Init" << std::endl;
				return false;
			}


			// ��ȡFaceMesh_Face_Mesh_Detect_Frame_New
			void* p_FaceMesh_Face_Mesh_Detect_Frame_New = dllLoader.GetFunction("FaceMesh_Face_Mesh_Detect_Frame_New");
			if (p_FaceMesh_Face_Mesh_Detect_Frame_New != nullptr)
			{
				FaceMesh_Face_Mesh_Detect_Frame_New = (FunC_FaceMesh_Face_Mesh_Detect_Frame_New(p_FaceMesh_Face_Mesh_Detect_Frame_New));
				if (FaceMesh_Face_Mesh_Detect_Frame_New != nullptr)
				{
					std::cout << "[CallBack] FaceMesh_Face_Mesh_Detect_Frame_New��ȡ�ɹ�" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] �޷���dll���ҵ�FaceMesh_Face_Mesh_Detect_Frame_New" << std::endl;
				return false;
			}


			// ��ȡFaceMesh_Hand_Tracking_Release
			void* p_FaceMesh_Face_Mesh_Release = dllLoader.GetFunction("FaceMesh_Face_Mesh_Release");
			if (p_FaceMesh_Face_Mesh_Release != nullptr)
			{
				FaceMesh_Face_Mesh_Release = (FunC_FaceMesh_Face_Mesh_Release(p_FaceMesh_Face_Mesh_Release));
				if (FaceMesh_Face_Mesh_Release != nullptr)
				{
					std::cout << "[CallBack] FaceMesh_Face_Mesh_Release��ȡ�ɹ�" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] �޷���dll���ҵ�FaceMesh_Face_Mesh_Release" << std::endl;
				return false;
			}

		}
		else
		{
			std::cout << model_path << "\\" << dll_name<<" ����ʧ�ܣ�" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << model_path << "\\" << dll_name << " ������" << std::endl;
		return false;
	}



	string temp = model_path + "\\" + pbtxt_name;
	if (FaceMesh_Face_Mesh_Init(temp.c_str()))
	{
		std::cout << model_path << "\\" << pbtxt_name << " ��ʼ��ģ�ͳɹ�" << std::endl;
	}
	else
	{
		std::cout << model_path << "\\" << pbtxt_name << " ��ʼ��ģ��ʧ��" << std::endl;
		return false;
	}

	return true;
}

bool C_FaceMesh::getLandmarks(Mat img,bool multi_objects)
{
	landmarks.clear();
	Mat copyMat = img.clone();
	uchar* pImageData = copyMat.data;
	float *landmarks_ = nullptr;
	int size_ = 0;
	landmarks_ = FaceMesh_Face_Mesh_Detect_Frame_New((void*)pImageData, copyMat.cols, copyMat.rows, copyMat.channels(), landmarks_, size_, false);
	if (landmarks_)
	{
		landmarks.clear();
		if (!multi_objects)
		{
			int count = 0;
			Point temp;
			for (int i = 0; i < size_; i = i + 3)
			{
				if (count >= 468)
				{
					break;
					return true;
				}
				temp.x = landmarks_[i];
				temp.y = landmarks_[i + 1];

				landmarks.push_back(temp);
				count++;
			}
			if (landmarks.size()!= 468)return false;
			return true;
		}
		else
		{
			Point temp;
			for (int i = 0; i < size_; i = i + 3)
			{
				temp.x = landmarks_[i];
				temp.y = landmarks_[i + 1];
				landmarks.push_back(temp);
			}
			return true;
		}

	}
	else
		return false;
}
