// pose_tracking_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "DynamicModuleLoader.h"
#include "DynamicModuleLoader.cpp"
#include "C_PoseTracking.h"
#include "C_PoseTracking.cpp"
#include "stop_watch.h"
#include "stop_watch.cpp"


std::string GetProgramDir();//获得exe路径（多字符集）
void DrawPose(cv::Mat& temp_frame, std::vector<cv::Point> temp_points);

int main()
{
	vector<cv::Point2i> line_p;
	line_p.push_back(cv::Point2i(0, 1));
	line_p.push_back(cv::Point2i(1, 2));
	line_p.push_back(cv::Point2i(2, 3));
	line_p.push_back(cv::Point2i(3, 7));
	line_p.push_back(cv::Point2i(0, 4));
	line_p.push_back(cv::Point2i(4, 5));
	line_p.push_back(cv::Point2i(5, 6));
	line_p.push_back(cv::Point2i(6, 8));
	line_p.push_back(cv::Point2i(9, 10));
	line_p.push_back(cv::Point2i(11, 12));
	line_p.push_back(cv::Point2i(11, 13));
	line_p.push_back(cv::Point2i(13, 15));
	line_p.push_back(cv::Point2i(15, 17));
	line_p.push_back(cv::Point2i(15, 21));
	line_p.push_back(cv::Point2i(19, 21));
	line_p.push_back(cv::Point2i(17, 19));
	line_p.push_back(cv::Point2i(11, 23));
	line_p.push_back(cv::Point2i(12, 24));
	line_p.push_back(cv::Point2i(12, 14));
	line_p.push_back(cv::Point2i(14, 16));
	line_p.push_back(cv::Point2i(16, 22));
	line_p.push_back(cv::Point2i(16, 18));
	line_p.push_back(cv::Point2i(16, 20));
	line_p.push_back(cv::Point2i(18, 20));
	line_p.push_back(cv::Point2i(23, 24));
	line_p.push_back(cv::Point2i(24, 26));
	line_p.push_back(cv::Point2i(26, 28));
	line_p.push_back(cv::Point2i(28, 30));
	line_p.push_back(cv::Point2i(28, 32));
	line_p.push_back(cv::Point2i(30, 32));
	line_p.push_back(cv::Point2i(23, 25));
	line_p.push_back(cv::Point2i(25, 27));
	line_p.push_back(cv::Point2i(27, 29));
	line_p.push_back(cv::Point2i(27, 31));
	line_p.push_back(cv::Point2i(29, 31));

	std::string exe_path = GetProgramDir();
	
	C_PoseTracking *pose_detector = new C_PoseTracking(exe_path, "pose_tracking_cpu_dll_full_0507.dll", "pose_tracking_cpu.pbtxt");
	if (pose_detector->m_Init_PoseTracking_OK == false) {
		std::cout << "hand tracking init failed!" << endl;
		return 0;
	}
	cv::Mat frame, detect_frame;
	std::vector<cv::Point> temp_landmarks;

	//cv::VideoCapture cap(0);
	cv::VideoCapture cap("F:\\25-Gelly_AirConditioning\\DataBase\\WIN_20220721_10_50_44_Pro.mp4");
	if (!cap.isOpened()) {
		std::cout << "can not open camera!" << endl;
		return 0;
	}
	stop_watch cost_t;
	cv::namedWindow("detect_frame", 0);
	cv::resizeWindow("detect_frame", cv::Size(1920/2, 720/2));
	int i = 0;
	while (1) {
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		frame.copyTo(detect_frame);
		cost_t.restart();
		pose_detector->getLandmarks(detect_frame, true);
		temp_landmarks = pose_detector->landmarks;
		cost_t.stop();
		cout << "cost:" << cost_t.elapsed_ms() << endl;
		for (int i = 0; i < temp_landmarks.size(); i++) {
			cv::Point temp_p = temp_landmarks[i];
			if (temp_p.x != 0 && temp_p.y != 0) {
				//cv::putText(detect_frame, to_string(i), temp_p, 1, 2.0, cv::Scalar(0, 0, 255));
				cv::circle(detect_frame, temp_p, 5, cv::Scalar(0, 0, 255), -1);
			}
		}
		for (int i = 0; i < line_p.size(); i++) {
			if (line_p[i].x > temp_landmarks.size() || line_p[i].y > temp_landmarks.size()) {
				continue;
			}
			cv::Point p1 = temp_landmarks[line_p[i].x];
			cv::Point p2 = temp_landmarks[line_p[i].y];
			if (p1.x != 0 && p1.y != 0 && p2.x != 0 && p2.y != 0) {
				cv::line(detect_frame, p1, p2, cv::Scalar(0, 255, 0), 2);
			}
		}
		cv::imshow("detect_frame", detect_frame);
		if (i == 0) {
			cv::waitKey(0);
		}
		else {
			cv::waitKey(1);
		}
		i++;
	}
	pose_detector->~C_PoseTracking();
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

void DrawPose(cv::Mat& temp_frame, std::vector<cv::Point> temp_points) {
	std::vector<cv::Point2i> line_id;
	line_id.push_back(cv::Point2i(11, 12));
	line_id.push_back(cv::Point2i(11, 13));
	line_id.push_back(cv::Point2i(13, 15));
	line_id.push_back(cv::Point2i(12, 14));
	line_id.push_back(cv::Point2i(14, 16));
	line_id.push_back(cv::Point2i(12, 24));
	line_id.push_back(cv::Point2i(11, 23));
	line_id.push_back(cv::Point2i(23, 24));
	line_id.push_back(cv::Point2i(24, 26));
	line_id.push_back(cv::Point2i(23, 25));
	line_id.push_back(cv::Point2i(26, 28));
	line_id.push_back(cv::Point2i(25, 27));
	if (temp_points.size() == 33) {
		for (int i = 0; i < line_id.size(); i++) {
			cv::Point2i p1 = temp_points[line_id[i].x];
			cv::Point2i p2 = temp_points[line_id[i].y];
			if (p1.x > 0 && p1.y > 0 && p2.x > 0 && p2.y > 0) {
				cv::line(temp_frame, p1, p2, cv::Scalar(0, 0, 255), 2);
			}
		}
		for (int i = 0; i < temp_points.size(); i++) {
			if (temp_points[i].x > 0 && temp_points[i].y > 0) {
				cv::circle(temp_frame, temp_points[i], 5, cv::Scalar(0, 255, 0), -1);
			}
		}
	}
}