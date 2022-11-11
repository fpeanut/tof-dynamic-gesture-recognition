// iris_tracking_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <io.h>
#include "DynamicModuleLoader.h"
#include "DynamicModuleLoader.cpp"
#include "C_IrisTracking.h"
#include "C_IrisTracking.cpp"
#include "stop_watch.h"
#include "stop_watch.cpp"


std::string GetProgramDir();//获得exe路径（多字符集）
void GetAllFiles(const std::string path, std::vector<std::string> &files);


int main()
{
	std::string exe_path = GetProgramDir();
	C_IrisTracking *iris_detector = new C_IrisTracking(exe_path, "iris_tracking_cpu_dll.dll", "iris_tracking_cpu.pbtxt");
	if (iris_detector->m_Init_IrisTracking_OK == false) {
		std::cout << "iris tracking init failed!" << endl;
		return 0;
	}
	cv::Mat frame, detect_frame;
	std::vector<cv::Point> temp_landmarks;
	stop_watch cost_t;
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return 1;
	}
	while (1) {
		cap >> frame;
		frame.copyTo(detect_frame);
		cost_t.restart();
		iris_detector->getLandmarks(detect_frame);
		temp_landmarks = iris_detector->landmarks;
		cost_t.stop();
		cout << "cost:" << cost_t.elapsed_ms() << endl;
		for (int i = 0; i < temp_landmarks.size(); i++) {
			cv::Point temp_p = temp_landmarks[i];
			cv::circle(detect_frame, temp_p, 1, cv::Scalar(0, 0, 255), -1);
		}
		cv::imshow("detect_frame", detect_frame);
		int k = cv::waitKey(20);
		if (k == 27) {
			break;
		}
	}	
	iris_detector->~C_IrisTracking();
	return 1;
}


std::string GetProgramDir()
{
	char exeFullPath[MAX_PATH]; // Full path
	std::string strPath = "";
	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH); //获取带有可执行文件名路径
	strPath = (std::string)exeFullPath;
	int pos = strPath.find_last_of('\\', strPath.length());
	return strPath.substr(0, pos);  // 返回不带有可执行文件名的路径
}

void GetAllFiles(const std::string path, std::vector<std::string> &files)
{
	//文件句柄
	intptr_t hFile = 0;
	//文件信息，_finddata_t需要io.h头文件
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				//在判断有无子目录的if分支中，由于系统在进入一个子目录时，匹配到的头两个文件(夹)是"."(当前目录)，".."(上一层目录)。
				//需要忽略掉这两种情况。当需要对遍历到的文件做处理时，在else分支中添加相应的代码就好
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetAllFiles(p.assign(path).append("/").append(fileinfo.name), files);
			}
			else
			{
				string name = p.assign(path).append("/").append(fileinfo.name);
				if ((name.substr(name.length() - 4, name.length()) == ".jpg") ||
					(name.substr(name.length() - 4, name.length()) == ".bmp") ||
					(name.substr(name.length() - 4, name.length()) == ".png") ||
					(name.substr(name.length() - 4, name.length()) == ".gif") ||
					(name.substr(name.length() - 5, name.length()) == ".tiff"))
				{
					files.push_back(name);
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}