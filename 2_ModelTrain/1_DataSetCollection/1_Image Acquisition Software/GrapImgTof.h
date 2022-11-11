#pragma once
#pragma execution_character_set("utf-8")//中文乱码

#include <QtWidgets/QWidget>
#include "ui_GrapImgTof.h"
//QT头文件
#include "QtCommonHeader.h"
#include "QtCommonFunction.h"
//Oflim相机Tof类
#include "OflimListener.h"
#include <QDebug>
#include<QThread>


class GrapImgTof : public QWidget
{
	Q_OBJECT

public:
	GrapImgTof(QWidget *parent = Q_NULLPTR);

	std::vector<int> jpg_quality;//给void saveJPG()使用
	QDateTime dataTime;
	QTime startTime, endTime;//等待时间戳
	QTimer *timer_;
	QTime time_;//定时器
	int time_count = 0;
	QString path;//外围文件生成路径
	bool m_open_flag = false;//是否打开
	bool m_save_flag = false;//是否保存
	bool horizontal_flip = false;//水平翻转
	bool vertical_flip = false;//垂直翻转
	int counts = 0, id = 0;//图像帧数，文件夹数
	QString m_exe_path;//exe文件路径
	QString m_config_path;//config路径
	QString m_hand_items;//config手势动作
	
	/*界面*/
	int m_tof_num = 0;//tof数量
	/*相机*/
	OflimListener* m_oflim_listener = new OflimListener();//多tof相机类
	//(a)界面参数初始化
	void initUiParam();
	//(b)获取图像保存地址
	QString GetSavePath();
	//(c)显示Mat图像
	void ShowMat(cv::Mat image, QLabel *show_image);
	//(d)读取显示数据
	void ReadData();//读取实时读取tof
	//(f)关闭窗口事件
	void closeEvent(QCloseEvent *event);//4)关闭窗口
	
public slots:
	//1、寻找相机设备
	void SearchClicked();
	//2、打开第一个相机
	void OpenFirstCam();
	//3、开始保存
	void StartSaveClicked();
	//4、创建文件夹
	void CreateDir();
	//5、定时器
	void update();
	//6.按键效果
	void on_pushButton_pressed();
	void on_pushButton_released();
	void on_pushButton1_pressed();
	void on_pushButton1_released();




private:
	Ui::GrapImgTofClass *ui;
};
