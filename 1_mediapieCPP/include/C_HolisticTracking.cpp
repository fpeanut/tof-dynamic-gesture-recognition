#include "C_HolisticTracking.h"


C_HolisticTracking::C_HolisticTracking(std::string _model_path, std::string _dll_name, std::string _pbtxt_name)
{
	model_path = _model_path;
	dll_name = _dll_name;
	pbtxt_name = _pbtxt_name;
	if (InitHolisticTracking())
	{
		m_Init_HolisticTracking_OK = true;
		cout << "Init HolisticTracking: True" << endl;
	}
	else
	{
		m_Init_HolisticTracking_OK = false;
		cout << "Init HolisticTracking: False" << endl;
	}
}

//��������
C_HolisticTracking::~C_HolisticTracking() {
	HolisticTracking_Holistic_Tracking_Release();
}

bool C_HolisticTracking::InitHolisticTracking()
{
	if (dllLoader.IsFileExist(model_path + "\\" + dll_name))
	{
		std::cout << "dll����:" << model_path << "\\" << dll_name << std::endl;

		if (dllLoader.LoadDynamicModule(model_path + "\\" + dll_name))
		{
			std::cout << "dll���سɹ���" << std::endl;
			// ��ȡHolisticTracking_Holistic_Tracking_Init
			void* p_HolisticTracking_Holistic_Tracking_Init = dllLoader.GetFunction("HolisticTracking_Holistic_Tracking_Init");
			if (p_HolisticTracking_Holistic_Tracking_Init != nullptr)
			{
				HolisticTracking_Holistic_Tracking_Init = (FunC_MediapipeHolisticTrackingInit(p_HolisticTracking_Holistic_Tracking_Init));
				if (HolisticTracking_Holistic_Tracking_Init != nullptr)
				{
					std::cout << "[CallBack] HolisticTracking_Holistic_Tracking_Init��ȡ�ɹ�" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] �޷���dll���ҵ�HolisticTracking_Holistic_Tracking_Init" << std::endl;
				return false;
			}

			// ��ȡHolisticTracking_Holistic_Tracking_Detect_Frame_New
			void* p_HolisticTracking_Holistic_Tracking_Detect_Frame_New = dllLoader.GetFunction("HolisticTracking_Holistic_Tracking_Detect_Frame_New");
			if (p_HolisticTracking_Holistic_Tracking_Detect_Frame_New != nullptr)
			{
				HolisticTracking_Holistic_Tracking_Detect_Frame_New = (FunC_HolisticTracking_Holistic_Tracking_Detect_Frame_New(p_HolisticTracking_Holistic_Tracking_Detect_Frame_New));
				if (HolisticTracking_Holistic_Tracking_Detect_Frame_New != nullptr)
				{
					std::cout << "[CallBack] HolisticTracking_Holistic_Tracking_Detect_Frame_New��ȡ�ɹ�" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] �޷���dll���ҵ�HolisticTracking_Holistic_Tracking_Detect_Frame_New" << std::endl;
				return false;
			}


			// ��ȡHolisticTracking_Holistic_Tracking_Release
			void* p_HolisticTracking_Holistic_Tracking_Release = dllLoader.GetFunction("HolisticTracking_Holistic_Tracking_Release");
			if (p_HolisticTracking_Holistic_Tracking_Release != nullptr)
			{
				HolisticTracking_Holistic_Tracking_Release = (FunC_HolisticTracking_Holistic_Tracking_Release(p_HolisticTracking_Holistic_Tracking_Release));
				if (HolisticTracking_Holistic_Tracking_Release != nullptr)
				{
					std::cout << "[CallBack] HolisticTracking_Holistic_Tracking_Release��ȡ�ɹ�" << std::endl;
				}
			}
			else
			{
				std::cout << "[CallBack] �޷���dll���ҵ�HolisticTracking_Holistic_Tracking_Release" << std::endl;
				return false;
			}

		}
		else
		{
			std::cout << model_path << "\\" << dll_name << " ����ʧ�ܣ�" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << model_path << "\\" << dll_name << " ������" << std::endl;
		return false;
	}



	string temp = model_path + "\\" + pbtxt_name;
	if (HolisticTracking_Holistic_Tracking_Init(temp.c_str()))
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

bool C_HolisticTracking::getLandmarks(Mat img)
{
	face_landmarks.clear();
	pose_landmarks.clear();
	left_landmarks.clear();
	right_landmarks.clear();
	Mat copyMat = img.clone();
	uchar* pImageData = copyMat.data;
	float *landmarks_ = nullptr;
	int face_size = 0;
	int pose_size = 0;
	int left_size = 0;
	int right_size = 0;
	landmarks_ = HolisticTracking_Holistic_Tracking_Detect_Frame_New(copyMat.cols, copyMat.rows, copyMat.channels(), (void*)pImageData, landmarks_, face_size, pose_size, left_size, right_size, false);
	if (landmarks_)
	{
		Point temp;
		for (int i = 0; i < face_size; i = i + 2) {
			temp.x = landmarks_[i];
			temp.y = landmarks_[i + 1];
			face_landmarks.push_back(temp);
		}
		for (int i = 0; i < pose_size; i = i + 2) {
			temp.x = landmarks_[face_size + i];
			temp.y = landmarks_[face_size + i + 1];
			pose_landmarks.push_back(temp);
		}
		for (int i = 0; i < left_size;i = i+2) {
			temp.x = landmarks_[face_size + pose_size+i];
			temp.y = landmarks_[face_size + pose_size+i + 1];
			left_landmarks.push_back(temp);
		}
		for (int i = 0; i < right_size; i = i + 2) {
			temp.x = landmarks_[face_size + pose_size + left_size + i];
			temp.y = landmarks_[face_size + pose_size + left_size + i + 1];
			right_landmarks.push_back(temp);
		}
		return true;
	}
	else
		return false;
}
