#pragma once
#pragma execution_character_set("utf-8")//��������

#include <QtWidgets/QWidget>
#include "ui_GetHandFeature.h"
#include <string> 
#include<qdebug.h>
#include <QFileInfo>
#include<iostream>
#include<fstream>
#include <math.h>
#include<vector>
//OpenCVͷ�ļ�
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
//QTͷ�ļ�
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

	std::vector<int> jpg_quality;//��void saveJPG()ʹ��
	/*����*/
	QString m_exe_path;//exe����·��
	QString m_config_path;//config·��
	int len_sequence;//�������ڿ��
	int slidStep;//��������
	/*Config����ֵ*/
	QString m_depth_name;
	QString m_ir_name;
	QString m_gesture_dll;
	QString m_gesture_pbtxt;
	/*�ļ��д���*/
	QString allSampledir;//�������������
	QString oneSampledir;//���������
	/*�ļ���ɾ��*/
	vector<QString>VecDelDir;//ɾ���쳣����
	vector<QString>VecDelLabelDir;//ɾ���쳣������ǩtxt
	

	/*����*/
	bool m_init_flag = false;
	C_HandTracking *m_hand_detector = nullptr;//�ֲ������
	/*���ݼ�*/
	QString m_data_path;//��Ч���ݼ���·��
	QString m_save_path;//����������·��
	std::vector<QString> m_data_names;//��Ч���ݼ���
	std::vector<std::string> m_depth_imgs;//Depthͼ���
	std::vector<std::string> m_ir_imgs;//IRͼ���
	/*�������*/
	bool m_start_flag = false;//�Ƿ�ʼ
	/*ϵͳ����*/
	void InitParam();//��ʼ������
	void InitWidget();//��ʼ������
	void closeEvent(QCloseEvent *event);//�رմ���
	/*�㷨���*/
	void CheckDataValid(QString file_path, std::vector<QString>& child_names);//�������ݼ���Ч��
	void Init_HandTracking();//��ʼ��HandTracking
	void ReadData_SaveTxt();//����
	
	
public slots:
	void hand_detect_clicked();//��ʼ
	void btn_ChooseData();//ѡ�����ݼ�
	void hand_sample_del();//ɾ���쳣����
	
	

private:
	Ui::GetHandFeatureClass *ui;
};
