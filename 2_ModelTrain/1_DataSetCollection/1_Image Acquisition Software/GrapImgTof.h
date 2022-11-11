#pragma once
#pragma execution_character_set("utf-8")//��������

#include <QtWidgets/QWidget>
#include "ui_GrapImgTof.h"
//QTͷ�ļ�
#include "QtCommonHeader.h"
#include "QtCommonFunction.h"
//Oflim���Tof��
#include "OflimListener.h"
#include <QDebug>
#include<QThread>


class GrapImgTof : public QWidget
{
	Q_OBJECT

public:
	GrapImgTof(QWidget *parent = Q_NULLPTR);

	std::vector<int> jpg_quality;//��void saveJPG()ʹ��
	QDateTime dataTime;
	QTime startTime, endTime;//�ȴ�ʱ���
	QTimer *timer_;
	QTime time_;//��ʱ��
	int time_count = 0;
	QString path;//��Χ�ļ�����·��
	bool m_open_flag = false;//�Ƿ��
	bool m_save_flag = false;//�Ƿ񱣴�
	bool horizontal_flip = false;//ˮƽ��ת
	bool vertical_flip = false;//��ֱ��ת
	int counts = 0, id = 0;//ͼ��֡�����ļ�����
	QString m_exe_path;//exe�ļ�·��
	QString m_config_path;//config·��
	QString m_hand_items;//config���ƶ���
	
	/*����*/
	int m_tof_num = 0;//tof����
	/*���*/
	OflimListener* m_oflim_listener = new OflimListener();//��tof�����
	//(a)���������ʼ��
	void initUiParam();
	//(b)��ȡͼ�񱣴��ַ
	QString GetSavePath();
	//(c)��ʾMatͼ��
	void ShowMat(cv::Mat image, QLabel *show_image);
	//(d)��ȡ��ʾ����
	void ReadData();//��ȡʵʱ��ȡtof
	//(f)�رմ����¼�
	void closeEvent(QCloseEvent *event);//4)�رմ���
	
public slots:
	//1��Ѱ������豸
	void SearchClicked();
	//2���򿪵�һ�����
	void OpenFirstCam();
	//3����ʼ����
	void StartSaveClicked();
	//4�������ļ���
	void CreateDir();
	//5����ʱ��
	void update();
	//6.����Ч��
	void on_pushButton_pressed();
	void on_pushButton_released();
	void on_pushButton1_pressed();
	void on_pushButton1_released();




private:
	Ui::GrapImgTofClass *ui;
};
