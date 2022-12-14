// holistic_tracking.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <fstream>
#include <iostream>
#include <windows.h>
#include <direct.h>
#include "DynamicModuleLoader.h"
#include "DynamicModuleLoader.cpp"
#include "C_HolisticTracking.h"
#include "C_HolisticTracking.cpp"
#include "stop_watch.h"
#include "stop_watch.cpp"


std::string GetProgramDir();//获得exe路径（多字符集）
void DrawFace(cv::Mat& temp_frame, std::vector<cv::Point> temp_points);
void DrawPose(cv::Mat& temp_frame, std::vector<cv::Point> temp_points);
void DrawHand(cv::Mat& temp_frame, std::vector<cv::Point> temp_points);
void AssignRGB(int pixel_value, int &red, int &green, int &blue, float nearDist, float farDist);//根据像素值得到对应RGB通道值
cv::Mat GrayToRGB(cv::Mat frame_gray, float nearDist, float farDist);//灰度图像转伪彩图像
void GammaTransform(const cv::Mat srcImage, cv::Mat& dstImage, double gamma);//伽马增强

int main()
{
	std::string exe_path = GetProgramDir();
	C_HolisticTracking *holistic_detector = new C_HolisticTracking(exe_path,
		"holistic_tracking_cpu_dll.dll", "holistic_tracking_cpu.pbtxt");
	if (holistic_detector->m_Init_HolisticTracking_OK == false) {
		std::cout << "holistic tracking init failed!" << endl;
		system("pause");
		return 0;
	}
	std::vector<cv::Point> face_landmarks;
	std::vector<cv::Point> pose_landmarks;
	std::vector<cv::Point> left_landmarks;
	std::vector<cv::Point> right_landmarks;

	//1)读取图像数据集
	cv::Mat ir_image, ir_bgr;
	std::vector<cv::Point> temp_landmarks;
	stop_watch cost_t;
	vector<cv::String> depth_images;
	vector<cv::String> ir_images;
	std::string file_path = "E:\\16-SOC\\2022_07_08_35CM_HandGesture_IR\\changsuo";
	std::string image_path = file_path + "\\IR";
	std::string result_path = file_path + "\\Holistic_Result";
	::_mkdir(result_path.c_str());
	cv::glob(image_path + "\\*.jpg", ir_images);
	cv::namedWindow("ir_bgr", 0);
	cv::moveWindow("ir_bgr", 500, 50);
	for (int i = 0; i < ir_images.size(); i++) {
		ir_image = cv::imread(ir_images[i].c_str(), 0);
		cv::cvtColor(ir_image, ir_bgr, cv::COLOR_GRAY2BGR);
		cost_t.restart();
		//获取点
		holistic_detector->getLandmarks(ir_bgr);
		face_landmarks = holistic_detector->face_landmarks;
		pose_landmarks = holistic_detector->pose_landmarks;
		left_landmarks = holistic_detector->left_landmarks;
		right_landmarks = holistic_detector->right_landmarks;
		cost_t.stop();
		cout << "cost:" << cost_t.elapsed_ms() <<endl;

		DrawFace(ir_bgr, face_landmarks);
		DrawPose(ir_bgr, pose_landmarks);
		DrawHand(ir_bgr, left_landmarks);
		DrawHand(ir_bgr, right_landmarks);

		cv::imshow("ir_bgr", ir_bgr);
		std::string save_path = result_path + "\\" + to_string(i) + ".jpg";
		cv::imwrite(save_path, ir_bgr);
		if (i == 0) {
			cv::waitKey(0);
		}
		int k = cv::waitKey(1);
		if (k == 27) {
			holistic_detector->~C_HolisticTracking();
			return 1;
		}
	}
	holistic_detector->~C_HolisticTracking();


	//cv::Mat frame, detect_frame;
	//stop_watch cost_t;
	//VideoCapture cap(0);
	//if (!cap.isOpened()) {
	//	return 1;
	//}
	//while(1){
	//	cap >> frame;
	//	frame.copyTo(detect_frame);
	//	cost_t.restart();
	//	holistic_detector->getLandmarks(detect_frame);
	//	face_landmarks = holistic_detector->face_landmarks;
	//	pose_landmarks = holistic_detector->pose_landmarks;
	//	left_landmarks = holistic_detector->left_landmarks;
	//	right_landmarks = holistic_detector->right_landmarks;
	//	cost_t.stop();
	//	cout << "cost:" << cost_t.elapsed_ms() <<endl;

	//	//3-4)绘图
	//	DrawFace(detect_frame, face_landmarks);
	//	DrawPose(detect_frame, pose_landmarks);
	//	DrawHand(detect_frame, left_landmarks);
	//	DrawHand(detect_frame, right_landmarks);
	//	cv::namedWindow("detect_frame", 0);
	//	cv::resizeWindow("detect_frame", cv::Size(960, 540));
	//	cv::imshow("detect_frame", detect_frame);
	//	int k = cv::waitKey(20);
	//	if (k == 27) {
	//		break;
	//	}
	//}
	//holistic_detector->~C_HolisticTracking();

	system("pause");
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

//根据像素值得到对应RGB通道值
void AssignRGB(int pixel_value, int &red, int &green, int &blue, float nearDist, float farDist)
{
	if (pixel_value > nearDist && pixel_value < farDist)
	{
		float x = 1.0 * (pixel_value - nearDist) / (farDist - nearDist); //normalization
		red = 255 * (x < 0.5 ? 1 : -(2.0 * x) + 2);
		green = 255 * (x < 0.5 ? (2.0 * x) : 1);
		blue = 0;
	}
	else if (pixel_value <= nearDist)
	{
		red = 0xFF;
		green = 0xFF;
		blue = 0xFF;
	}
	else if (pixel_value >= farDist)
	{
		red = 0;
		green = 0;
		blue = 0;
	}
}

//灰度图像转伪彩图像
cv::Mat GrayToRGB(cv::Mat frame_gray, float nearDist, float farDist) {
	cv::Mat rgb_image = cv::Mat::ones(frame_gray.size(), CV_8UC3);
	for (int i = 0; i < frame_gray.rows; i++) {
		for (int j = 0; j < frame_gray.cols; j++) {
			int depth = frame_gray.at<uchar>(i, j) / 255.0 * (farDist - nearDist) + nearDist;
			//伪彩三通道
			int red = 0, green = 0, blue = 0;
			if (depth < farDist && depth > nearDist) {
				AssignRGB(depth, red, green, blue, nearDist, farDist);
			}
			rgb_image.at<uchar>(i, j * 3 + 0) = blue;
			rgb_image.at<uchar>(i, j * 3 + 1) = green;
			rgb_image.at<uchar>(i, j * 3 + 2) = red;
		}
	}
	return rgb_image;
}

//伽马增强
void GammaTransform(const cv::Mat srcImage, cv::Mat& dstImage, double gamma)
{
	unsigned char lut[256];
	for (int i = 0; i < 256; i++)
	{
		lut[i] = cv::saturate_cast<uchar>(pow((float)i / 255.0, gamma) * 255.0f);
	}
	dstImage = srcImage.clone();
	int channels = srcImage.channels();
	switch (channels)
	{
	case 1:
	{
		cv::MatIterator_<uchar> it = dstImage.begin<uchar>();
		cv::MatIterator_<uchar> end = dstImage.end<uchar>();
		while (it != end)
		{
			*it = lut[(*it)];
			it++;
		}
		break;
	}
	case 3:
	{
		cv::MatIterator_<cv::Vec3b> it = dstImage.begin<cv::Vec3b>();
		cv::MatIterator_<cv::Vec3b> end = dstImage.end<cv::Vec3b>();
		while (it != end)
		{
			(*it)[0] = lut[(*it)[0]];
			(*it)[1] = lut[(*it)[1]];
			(*it)[2] = lut[(*it)[2]];
			it++;
		}
		break;
	}
	default:
		break;
	}
}

void DrawFace(cv::Mat& temp_frame, std::vector<cv::Point> temp_points) {
	if (temp_points.size() == 468) {
		for (int i = 0; i < temp_points.size(); i++) {
			cv::circle(temp_frame, temp_points[i], 1, cv::Scalar(0, 255, 0), -1);
		}
	}
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

void DrawHand(cv::Mat& temp_frame, std::vector<cv::Point> temp_points) {
	if (temp_points.size() == 21) {
		//连线
		vector<Point2i> line_points = {
			Point2i(0, 1), Point2i(1, 2), Point2i(2, 3), Point2i(3, 4),
			Point2i(1, 5), Point2i(5, 6), Point2i(6, 7), Point2i(7, 8),
			Point2i(5, 9), Point2i(9, 10), Point2i(10, 11), Point2i(11, 12),
			Point2i(9, 13), Point2i(13, 14), Point2i(14, 15), Point2i(15, 16),
			Point2i(13, 17), Point2i(17, 18), Point2i(18, 19), Point2i(19, 20),
			Point2i(17, 0)
		};
		for (int i = 0; i < line_points.size(); i++) {
			Point2i p1 = temp_points[line_points[i].x];
			Point2i p2 = temp_points[line_points[i].y];
			if (p1.x > 0 && p1.y > 0 && p2.x > 0 && p2.y > 0)
				cv::line(temp_frame, p1, p2, cv::Scalar(0, 0, 255), 2, 8, 0);
		}
		//显示骨架点
		for (int i = 0; i < temp_points.size(); i++) {
			cv::circle(temp_frame, temp_points[i], 5, cv::Scalar(255, 255, 255), cv::FILLED);
		}
	}

}