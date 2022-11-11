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
#include<vector>
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

	std::vector<int> jpg_quality;//给void saveJPG()使用
	/*界面*/
	QString m_exe_path;//exe运行路径
	QString m_config_path;//config路径
	int len_sequence;//滑动窗口宽度
	int slidStep;//滑动步长
	/*Config参数值*/
	QString m_depth_name;
	QString m_ir_name;
	QString m_gesture_dll;
	QString m_gesture_pbtxt;
	/*文件夹创建*/
	QString allSampledir;//单类别所有样本
	QString oneSampledir;//单类别单样本
	/*文件夹删除*/
	vector<QString>VecDelDir;//删除异常样本
	vector<QString>VecDelLabelDir;//删除异常样本标签txt
	

	/*手势*/
	bool m_init_flag = false;
	C_HandTracking *m_hand_detector = nullptr;//手部检测器
	/*数据集*/
	QString m_data_path;//有效数据集的路径
	QString m_save_path;//保存特征的路径
	std::vector<QString> m_data_names;//有效数据集名
	std::vector<std::string> m_depth_imgs;//Depth图像库
	std::vector<std::string> m_ir_imgs;//IR图像库
	/*保存输出*/
	bool m_start_flag = false;//是否开始
	/*系统函数*/
	void InitParam();//初始化参数
	void InitWidget();//初始化界面
	void closeEvent(QCloseEvent *event);//关闭窗口
	/*算法相关*/
	void CheckDataValid(QString file_path, std::vector<QString>& child_names);//核验数据集有效性
	void Init_HandTracking();//初始化HandTracking
	void ReadData_SaveTxt();//保存
	
	
public slots:
	void hand_detect_clicked();//开始
	void btn_ChooseData();//选择数据集
	void hand_sample_del();//删除异常样本
	
	

private:
	Ui::GetHandFeatureClass *ui;
};
