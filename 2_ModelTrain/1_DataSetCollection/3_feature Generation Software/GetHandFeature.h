#pragma once
#pragma execution_character_set("utf-8")//中文乱码

#include <QtWidgets/QWidget>
#include "ui_GetHandFeature.h"
#include <string> 
#include<qdebug.h>
#include <QFileInfo>
#include<iostream>
#include<fstream>
#include <math.h>
//OpenCV头文件
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
//QT头文件
#include "QtCommonHeader.h"
#include "QtCommonFunction.h"
//HandTracking
#include "DynamicModuleLoader.h"
#include "C_HandTracking.h"

class GetHandFeature : public QWidget
{
	Q_OBJECT

public:
	GetHandFeature(QWidget *parent = Q_NULLPTR);
	~GetHandFeature();

	/*界面*/
	QString m_exe_path;//exe运行路径
	QString m_config_path;//config路径
	QString m_trainPath_label;//数据集标签路径
	QString m_trainPath_points2d;//数据集2d路径
	QString m_trainPath_points3d;//数据集3d路径
	QString m_trainPath_points3dn;//数据集3dnormal路径
	QString m_trainPath_MixedFeatures;//数据集混合特征路径
	bool dircreateFlag = false;
	/*Config参数值*/
	QString m_depth_name;
	QString m_ir_name;
	QString m_gesture_dll;
	QString m_gesture_pbtxt;
	/*3D区间*/
	int m_min_x, m_max_x;
	int m_min_y, m_max_y;
	int m_min_z, m_max_z;
	/*手势*/
	bool m_init_flag = false;
	C_HandTracking *m_hand_detector = nullptr;//手部检测器
	/*数据集*/
	QString m_data_path;//有效数据集的路径
	QString m_save_path;//保存特征的路径
	std::vector<QString> m_data_names;//有效数据集名
	std::vector<std::string> m_depth_imgs;//Depth图像库
	std::vector<std::string> m_ir_imgs;//IR图像库
	std::vector<cv::Point3f>pre_point3d;//前一帧点集
	std::vector<cv::Point>pre_point2d;//前一帧点集
	/*保存输出*/
	std::vector<int> m_waitkeys;//等待时间
	int m_wait_key;
	bool m_start_flag = false;//是否开始
	/*系统函数*/
	void InitParam();//初始化参数
	void InitWidget();//初始化界面
	void closeEvent(QCloseEvent *event);//关闭窗口
	/*算法相关*/
	void CheckDataValid(QString file_path, std::vector<QString>& child_names);//核验数据集有效性
	void Init_HandTracking();//初始化HandTracking
	std::vector<cv::Point3f> Extrac_Point3D(std::vector<cv::Point> points_2D, cv::Mat depth_mat, float fx, float fy,
		float cx, float cy, int near_dis, int far_dis);//提取手部3D点
	void ReadData_SaveTxt();//保存
	std::vector<cv::Point3f>Normalize_Point3D(std::vector<cv::Point3f>points_3D_);//手部3D点归一化
	std::vector<cv::Point2f> Normalize_Point2D(std::vector<cv::Point>points_2D_);//手部2D点归一化
	std::vector<cv::Point3f> GetHandFeature::Normalize_Point3Ddis(std::vector<cv::Point3f>points_3D_);//手部3d点的距离归一化
	std::vector<cv::Point2f> Normalize_Point2Ddis(std::vector<cv::Point2f>points_2D_);//手部2d点的距离归一化
	std::vector<float>Normalize_MotionDistance3(std::vector<float>p_);//3d距离归一化
	/*手工数据集*/
	std::vector<cv::Point3f>getRelativeDistance3d(std::vector<cv::Point3f>current, std::vector<cv::Point3f>last);//计算3d邻点相对距离
	std::vector<cv::Point2f>getRelativeDistance2d(std::vector<cv::Point>current, std::vector<cv::Point>last);//计算3d邻点相对距离
	std::vector<float>getMotionDistance3D(std::vector<cv::Point3f>p_);//计算3d邻点移动距离
	
public slots:
	void hand_detect_clicked();//开始
	void btn_ChooseData();//选择数据集
	

private:
	Ui::GetHandFeatureClass *ui;
};
