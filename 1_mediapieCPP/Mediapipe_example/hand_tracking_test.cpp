// hand_tracking_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <fstream>
#include <iostream>
#include <windows.h>
#include <direct.h>
#include "DynamicModuleLoader.h"
#include "DynamicModuleLoader.cpp"
#include "C_HandTracking.h"
#include "C_HandTracking.cpp"
#include "stop_watch.h"
#include "stop_watch.cpp"


std::string GetProgramDir();//获得exe路径（多字符集）
void DrawHand(cv::Mat& temp_image, std::vector<cv::Point> temp_points);
void AssignRGB(int pixel_value, int &red, int &green, int &blue, float nearDist, float farDist);//根据像素值得到对应RGB通道值
cv::Mat GrayToRGB(cv::Mat frame_gray, float nearDist, float farDist);//灰度图像转伪彩图像
void GammaTransform(const cv::Mat srcImage, cv::Mat& dstImage, double gamma);//伽马增强

int main()
{
	std::string exe_path = GetProgramDir();

	C_HandTracking *hand_detector = new C_HandTracking(exe_path, "hand_tracking_cpu_dll_lite.dll", "hand_tracking_desktop_live.pbtxt");
	if (hand_detector->m_Init_HandTracking_OK == false) {
		std::cout << "hand tracking init failed!" << endl;
		return 0;
	}
	
	//1)读取图像数据集
	cv::Mat ir_image, ir_bgr;
	std::vector<cv::Point> temp_landmarks;
	stop_watch cost_t;
	vector<cv::String> ir_images;
	std::string file_path = "E:\\16-SOC\\2022_07_08_35CM_HandGesture_IR\\changsuo";
	std::string image_path = file_path + "\\IR";
	std::string result_path = file_path + "\\Hand_Result";
	::_mkdir(result_path.c_str());
	cv::glob(image_path + "\\*.jpg", ir_images);
	cv::namedWindow("ir_image", 0);
	cv::moveWindow("ir_image", 500, 50);
	for (int i = 0; i < ir_images.size(); i++) {
		ir_image = cv::imread(ir_images[i].c_str(), 0);

		// GammaTransform(ir_image, ir_bgr, 0.8);
		cv::cvtColor(ir_image, ir_bgr, cv::COLOR_GRAY2BGR);
		//获取点
		hand_detector->getLandmarks(ir_bgr, false);
		temp_landmarks = hand_detector->landmarks;

		DrawHand(ir_bgr, temp_landmarks);
		cv::imshow("ir_image", ir_bgr);
		std::string save_path = result_path + "\\" + to_string(i) + ".jpg";
		cv::imwrite(save_path, ir_bgr);

		int k = cv::waitKey(1);
		if (k == 27) {
			hand_detector->~C_HandTracking();
			return 1;
		}
	}
	hand_detector->~C_HandTracking();
	
	////读取摄像头
	//cv::Mat frame, detect_frame;
	//std::vector<cv::Point> temp_landmarks;
	//stop_watch cost_t;
	//cv::VideoCapture cap(0);
	//if (!cap.isOpened()) {
	//	return 0;
	//}
	//while(1){
	//	cap >> frame;
	//	frame.copyTo(detect_frame);
	//	cost_t.restart();
	//	hand_detector->getLandmarks(detect_frame, true);
	//	temp_landmarks = hand_detector->landmarks;
	//	cost_t.stop();
	//	cout << "cost:" << cost_t.elapsed_ms() << endl;
	//	for (int i = 0; i < temp_landmarks.size(); i++) {
	//		cv::Point temp_p = temp_landmarks[i];
	//		cv::circle(detect_frame, temp_p, 5, cv::Scalar(0, 0, 255), -1);
	//	}
	//	cv::imshow("detect_frame", detect_frame);
	//	int k = cv::waitKey(20);
	//	if (k == 27) {
	//		break;
	//	}
	//}
	//hand_detector->~C_HandTracking();

	return 1;
}

//读取运行路径
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

//画出手部骨架
void DrawHand(cv::Mat& temp_image, std::vector<cv::Point> temp_points) {
	if (temp_points.size() != 21) {
		return;
	}
	//连线
	vector<cv::Point2i> line_points = {
		cv::Point2i(0, 1), cv::Point2i(1, 2), cv::Point2i(2, 3), cv::Point2i(3, 4),
		cv::Point2i(1, 5), cv::Point2i(5, 6), cv::Point2i(6, 7), cv::Point2i(7, 8),
		cv::Point2i(5, 9), cv::Point2i(9, 10), cv::Point2i(10, 11), cv::Point2i(11, 12),
		cv::Point2i(9, 13), cv::Point2i(13, 14), cv::Point2i(14, 15), cv::Point2i(15, 16),
		cv::Point2i(13, 17), cv::Point2i(17, 18), cv::Point2i(18, 19), cv::Point2i(19, 20),
		cv::Point2i(17, 0)
	};
	for (int i = 0; i < line_points.size(); i++) {
		cv::Point2i p1 = temp_points[line_points[i].x];
		cv::Point2i p2 = temp_points[line_points[i].y];
		cv::line(temp_image, p1, p2, cv::Scalar(0, 0, 255), 2, 8, 0);
	}
	//显示骨架点
	for (int i = 0; i < temp_points.size(); i++) {
		cv::circle(temp_image, temp_points[i], 5, cv::Scalar(255, 255, 255), cv::FILLED);
	}
}
