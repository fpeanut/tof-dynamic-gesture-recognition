#pragma once
#pragma execution_character_set("utf-8")//中文乱码
#include"RangeSlider.h"

#include <QtWidgets/QWidget>
#include "ui_SelectData.h"
#include<qmessagebox.h>
#include<qfile.h>
#include<QTime>
#include<iostream>
#include<QHBoxLayout>
//QT头文件
#include "QtCommonHeader.h"
#include "QtCommonFunction.h"
//opencv
#include<opencv2/opencv.hpp>
using namespace cv;


class SelectData : public QWidget
{
	Q_OBJECT

public:
	SelectData(QWidget *parent = Q_NULLPTR);
	/*界面*/
	QString m_exe_path;//exe运行路径
	QString m_config_path;//config路径
	int sliderLen = 10;
	/*Config参数值*/
	QString m_depth_name;
	QString m_ir_name;
	/*数据集*/
	QString temp_qstr;
	QString label_info;
	int img_index = 0;
	std::vector<std::string> m_depth_imgs;//Depth图像路径库
	std::vector<std::string> m_ir_imgs;//IR图像路径库
	std::vector<Mat> s_depth_imgs;//Depth图像库
	std::vector<Mat> s_ir_imgs;//IR图像库
	/*系统函数*/
	void InitParam();//初始化参数
	void closeEvent(QCloseEvent *event);//关闭窗口
	void saveData();//保存有效数据
	QString readTXT(QString txtPath);//读取label txt内容

public slots:
	void btn_ChooseData();//选择数据集
	void ReadImg();//读取图像
	void endclicked();//选择是否保存
	void brush_clicked();//擦除vector<Mat>数据
	void lowup_clicked();
	void lowdown_clicked();
	void upup_clicked();
	void updown_clicked();

private:
	Ui::SelectDataClass *ui;
	RangeSlider* rangeSliderThre;

};
