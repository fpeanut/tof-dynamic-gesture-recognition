#include "C_PoseTracking.h"


C_PoseTracking::C_PoseTracking(std::string _model_path, std::string _dll_name, std::string _pbtxt_name)
{
	model_path = _model_path;
	dll_name = _dll_name;
	pbtxt_name = _pbtxt_name;
	if (InitPoseTracking())
	{
		m_Init_PoseTracking_OK = true;
		cout << "Init PoseTracking: True" << endl;
	}
	else
	{
		m_Init_PoseTracking_OK = false;
		cout << "Init PoseTracking: False" << endl;
	}
}

C_PoseTracking::~C_PoseTracking() {
	PoseTracking_Pose_Tracking_Release();
}

bool C_PoseTracking::InitPoseTracking()
{
	if (dllLoader.IsFileExist(model_path + "\\" + dll_name))
	{
		std::cout << "dll存在:" << model_path << "\\" << dll_name << std::endl;

		if (dllLoader.LoadDynamicModule(model_path + "\\" + dll_name))
		{
			std::cout << "dll加载成功！" << std::endl;
			// 获取PoseTracking_Pose_Tracking_Init
			void* p_PoseTracking_Pose_Tracking_Init = dllLoader.GetFunction("PoseTracking_Pose_Tracking_Init");
			if (p_PoseTracking_Pose_Tracking_Init != nullptr)
			{
				PoseTracking_Pose_Tracking_Init = (FunC_PoseTracking_Pose_Tracking_Init(p_PoseTracking_Pose_Tracking_Init));
				if (PoseTracking_Pose_Tracking_Init != nullptr)
				{
					std::cout << "[CallBack] PoseTracking_Pose_Tracking_Init获取成功" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] 无法在dll中找到PoseTracking_Pose_Tracking_Init" << std::endl;
				return false;
			}

			// 获取PoseTracking_Pose_Tracking_Detect_Frame_New
			void* p_PoseTracking_Pose_Tracking_Detect_Frame_New = dllLoader.GetFunction("PoseTracking_Pose_Tracking_Detect_Frame_New");
			if (p_PoseTracking_Pose_Tracking_Detect_Frame_New != nullptr)
			{
				PoseTracking_Pose_Tracking_Detect_Frame_New = (FunC_PoseTracking_Pose_Tracking_Detect_Frame_New(p_PoseTracking_Pose_Tracking_Detect_Frame_New));
				if (PoseTracking_Pose_Tracking_Detect_Frame_New != nullptr)
				{
					std::cout << "[CallBack] PoseTracking_Pose_Tracking_Detect_Frame_New获取成功" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] 无法在dll中找到PoseTracking_Pose_Tracking_Detect_Frame_New" << std::endl;
				return false;
			}


			// 获取PoseTracking_Pose_Tracking_Release
			void* p_PoseTracking_Pose_Tracking_Release = dllLoader.GetFunction("PoseTracking_Pose_Tracking_Release");
			if (p_PoseTracking_Pose_Tracking_Release != nullptr)
			{
				PoseTracking_Pose_Tracking_Release = (FunC_PoseTracking_Pose_Tracking_Release(p_PoseTracking_Pose_Tracking_Release));
				if (PoseTracking_Pose_Tracking_Release != nullptr)
				{
					std::cout << "[CallBack] PoseTracking_Pose_Tracking_Release获取成功" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] 无法在dll中找到PoseTracking_Pose_Tracking_Release" << std::endl;
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
	if (PoseTracking_Pose_Tracking_Init(temp.c_str()))
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

bool C_PoseTracking::getLandmarks(Mat img, bool multi_objects)
{
	landmarks.clear();
	Mat copyMat = img.clone();
	uchar* pImageData = copyMat.data;
	float *landmarks_ = nullptr;
	int size_ = 0;
	landmarks_ = PoseTracking_Pose_Tracking_Detect_Frame_New(copyMat.cols, copyMat.rows, copyMat.channels(), (void*)pImageData, landmarks_, size_, false);
	if (landmarks_)
	{
		landmarks.clear();
		if (!multi_objects)
		{
			int count = 0;
			Point temp;
			for (int i = 0; i < size_; i = i + 2)
			{
				if (count > 33)
				{
					break;
					return true;
				}
				temp.x = landmarks_[i];
				temp.y = landmarks_[i + 1];

				landmarks.push_back(temp);
				count++;
			}
			if (landmarks.size() != 33)return false;
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
