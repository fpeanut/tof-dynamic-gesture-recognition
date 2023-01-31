#pragma once
#pragma execution_character_set("utf-8")//��������
#include"RangeSlider.h"

#include <QtWidgets/QWidget>
#include "ui_SelectData.h"
#include<qmessagebox.h>
#include<qfile.h>
#include<QTime>
#include<iostream>
#include<QHBoxLayout>
//QTͷ�ļ�
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
	/*����*/
	QString m_exe_path;//exe����·��
	QString m_config_path;//config·��
	int sliderLen = 10;
	/*Config����ֵ*/
	QString m_depth_name;
	QString m_ir_name;
	/*���ݼ�*/
	QString temp_qstr;
	QString label_info;
	int img_index = 0;
	std::vector<std::string> m_depth_imgs;//Depthͼ��·����
	std::vector<std::string> m_ir_imgs;//IRͼ��·����
	std::vector<Mat> s_depth_imgs;//Depthͼ���
	std::vector<Mat> s_ir_imgs;//IRͼ���
	/*ϵͳ����*/
	void InitParam();//��ʼ������
	void closeEvent(QCloseEvent *event);//�رմ���
	void saveData();//������Ч����
	QString readTXT(QString txtPath);//��ȡlabel txt����

public slots:
	void btn_ChooseData();//ѡ�����ݼ�
	void ReadImg();//��ȡͼ��
	void endclicked();//ѡ���Ƿ񱣴�
	void brush_clicked();//����vector<Mat>����
	void lowup_clicked();
	void lowdown_clicked();
	void upup_clicked();
	void updown_clicked();

private:
	Ui::SelectDataClass *ui;
	RangeSlider* rangeSliderThre;

};
