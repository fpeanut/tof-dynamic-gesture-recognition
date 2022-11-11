#include "C_HandTracking.h"


C_HandTracking::C_HandTracking(std::string _model_path, std::string _dll_name, std::string _pbtxt_name)
{
	model_path = _model_path;
	dll_name = _dll_name;
	pbtxt_name = _pbtxt_name;
	if (InitHandTracking())
	{
		m_Init_HandTracking_OK = true;
		cout << "Init HandTracking: True" << endl;
	}
	else
	{
		m_Init_HandTracking_OK = false;
		cout << "Init HandTracking: False" << endl;
	}
}

//析构函数
C_HandTracking::~C_HandTracking() {
	HandTracking_Hand_Tracking_Release();
}

bool C_HandTracking::InitHandTracking()
{
	if (dllLoader.IsFileExist(model_path + "\\" + dll_name))
	{
		if (dllLoader.LoadDynamicModule(model_path + "\\" + dll_name))
		{
			// 获取HandTracking_Hand_Tracking_Init
			void* p_HandTracking_Hand_Tracking_Init = dllLoader.GetFunction("HandTracking_Hand_Tracking_Init");
			if (p_HandTracking_Hand_Tracking_Init != nullptr)
			{
				HandTracking_Hand_Tracking_Init = (FunC_HandTracking_Hand_Tracking_Init(p_HandTracking_Hand_Tracking_Init));
				if (HandTracking_Hand_Tracking_Init != nullptr)
				{
					std::cout << "[CallBack] HandTracking_Hand_Tracking_Init success" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] HandTracking_Hand_Tracking_Init failed" << std::endl;
				return false;
			}

			// 获取HandTracking_Hand_Tracking_Detect_Frame_New
			void* p_HandTracking_Hand_Tracking_Detect_Frame_New = dllLoader.GetFunction("HandTracking_Hand_Tracking_Detect_Frame_New");
			if (p_HandTracking_Hand_Tracking_Detect_Frame_New != nullptr)
			{
				HandTracking_Hand_Tracking_Detect_Frame_New = (FunC_HandTracking_Hand_Tracking_Detect_Frame_New(p_HandTracking_Hand_Tracking_Detect_Frame_New));
				if (HandTracking_Hand_Tracking_Detect_Frame_New != nullptr)
				{
					std::cout << "[CallBack] HandTracking_Hand_Tracking_Detect_Frame_New success" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] HandTracking_Hand_Tracking_Detect_Frame_New failed" << std::endl;
				return false;
			}


			// 获取HandTracking_Hand_Tracking_Release
			void* p_HandTracking_Hand_Tracking_Release = dllLoader.GetFunction("HandTracking_Hand_Tracking_Release");
			if (p_HandTracking_Hand_Tracking_Release != nullptr)
			{
				HandTracking_Hand_Tracking_Release = (FunC_HandTracking_Hand_Tracking_Release(p_HandTracking_Hand_Tracking_Release));
				if (HandTracking_Hand_Tracking_Release != nullptr)
				{
					std::cout << "[CallBack] HandTracking_Hand_Tracking_Release success" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] HandTracking_Hand_Tracking_Release failed" << std::endl;
				return false;
			}

		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}



	string temp = model_path + "\\" + pbtxt_name;
	if (!HandTracking_Hand_Tracking_Init(temp.c_str()))
	{
		return false;
	}
	return true;
}

bool C_HandTracking::getLandmarks(Mat img, bool multi_objects)
{
	landmarks.clear();
	Mat copyMat = img.clone();
	uchar* pImageData = copyMat.data;
	float *landmarks_ = nullptr;
	int size_ = 0;
	landmarks_ = HandTracking_Hand_Tracking_Detect_Frame_New(copyMat.cols, copyMat.rows, copyMat.channels(), (void*)pImageData, landmarks_, size_, false);
	if (landmarks_)
	{
		landmarks.clear();
		if (!multi_objects)
		{
			int count = 0;
			Point temp;
			for (int i = 0; i < size_; i = i + 2)
			{
				if (count >= 21)
				{
					break;
					return true;
				}
				temp.x = landmarks_[i];
				temp.y = landmarks_[i + 1];

				landmarks.push_back(temp);
				count++;
			}
			if (landmarks.size() != 21)return false;
			return true;
		}
		else
		{
			Point temp;
			for (int i = 0; i < size_; i = i + 2)
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
