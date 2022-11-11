#include "C_IrisTracking.h"


C_IrisTracking::C_IrisTracking(std::string _model_path, std::string _dll_name, std::string _pbtxt_name)
{
	model_path = _model_path;
	dll_name = _dll_name;
	pbtxt_name = _pbtxt_name;
	if (InitIrisTracking())
	{
		m_Init_IrisTracking_OK = true;
		cout << "Init IrisTracking: True" << endl;
	}
	else
	{
		m_Init_IrisTracking_OK = false;
		cout << "Init IrisTracking: False" << endl;
	}
}

C_IrisTracking::~C_IrisTracking() {
	IrisTracking_Iris_Tracking_Release();
}

bool C_IrisTracking::InitIrisTracking()
{
	if (dllLoader.IsFileExist(model_path + "\\" + dll_name))
	{
		std::cout << "dll存在:" << model_path << "\\" << dll_name << std::endl;

		if (dllLoader.LoadDynamicModule(model_path + "\\" + dll_name))
		{
			std::cout << "dll加载成功！" << std::endl;
			// 获取IrisTracking_Iris_Tracking_Init
			void* p_IrisTracking_Iris_Tracking_Init = dllLoader.GetFunction("IrisTracking_Iris_Tracking_Init");
			if (p_IrisTracking_Iris_Tracking_Init != nullptr)
			{
				IrisTracking_Iris_Tracking_Init = (FunC_IrisTracking_Iris_Tracking_Init(p_IrisTracking_Iris_Tracking_Init));
				if (IrisTracking_Iris_Tracking_Init != nullptr)
				{
					std::cout << "[CallBack] IrisTracking_Iris_Tracking_Init获取成功" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] 无法在dll中找到IrisTracking_Iris_Tracking_Init" << std::endl;
				return false;
			}

			// 获取IrisTracking_Iris_Tracking_Detect_Frame_New
			void* p_IrisTracking_Iris_Tracking_Detect_Frame_New = dllLoader.GetFunction("IrisTracking_Iris_Tracking_Detect_Frame_New");
			if (p_IrisTracking_Iris_Tracking_Detect_Frame_New != nullptr)
			{
				IrisTracking_Iris_Tracking_Detect_Frame_New = (FunC_IrisTracking_Iris_Tracking_Detect_Frame_New(p_IrisTracking_Iris_Tracking_Detect_Frame_New));
				if (IrisTracking_Iris_Tracking_Detect_Frame_New != nullptr)
				{
					std::cout << "[CallBack] IrisTracking_Iris_Tracking_Detect_Frame_New获取成功" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] 无法在dll中找到IrisTracking_Iris_Tracking_Detect_Frame_New" << std::endl;
				return false;
			}


			// 获取IrisTracking_Iris_Tracking_Release
			void* p_IrisTracking_Iris_Tracking_Release = dllLoader.GetFunction("IrisTracking_Iris_Tracking_Release");
			if (p_IrisTracking_Iris_Tracking_Release != nullptr)
			{
				IrisTracking_Iris_Tracking_Release = (FunC_IrisTracking_Iris_Tracking_Release(p_IrisTracking_Iris_Tracking_Release));
				if (IrisTracking_Iris_Tracking_Release != nullptr)
				{
					std::cout << "[CallBack] IrisTracking_Iris_Tracking_Release获取成功" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] 无法在dll中找到IrisTracking_Iris_Tracking_Release" << std::endl;
				return false;
			}

		}
		else
		{
			std::cout << model_path << "\\" << dll_name << " 加载失败！" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << model_path << "\\" << dll_name << " 不存在" << std::endl;
		return false;
	}



	string temp = model_path + "\\" + pbtxt_name;
	if (IrisTracking_Iris_Tracking_Init(temp.c_str()))
	{
		std::cout << model_path << "\\" << pbtxt_name << " 初始化模型成功" << std::endl;
	}
	else
	{
		std::cout << model_path << "\\" << pbtxt_name << " 初始化模型失败" << std::endl;
		return false;
	}
	return true;
}

bool C_IrisTracking::getLandmarks(Mat img)
{
	landmarks.clear();
	Mat copyMat = img.clone();
	uchar* pImageData = copyMat.data;
	float *landmarks_ = nullptr;
	int size_ = 0;
	landmarks_ = IrisTracking_Iris_Tracking_Detect_Frame_New(copyMat.cols, copyMat.rows, copyMat.channels(), (void*)pImageData, landmarks_, size_, false);

	if (landmarks_)
	{
		landmarks.clear();
		Point temp;
		for (int i = 0; i < size_; i = i + 2)
		{
			temp.x = landmarks_[i];
			temp.y = landmarks_[i + 1];
			landmarks.push_back(temp);
		}
		return true;

	}
	else
		return false;
}
