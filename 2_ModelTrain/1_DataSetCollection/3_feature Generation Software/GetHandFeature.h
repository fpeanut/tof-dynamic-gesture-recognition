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

	/*����*/
	QString m_exe_path;//exe����·��
	QString m_config_path;//config·��
	QString m_trainPath_label;//���ݼ���ǩ·��
	QString m_trainPath_points2d;//���ݼ�2d·��
	QString m_trainPath_points3d;//���ݼ�3d·��
	QString m_trainPath_points3dn;//���ݼ�3dnormal·��
	QString m_trainPath_MixedFeatures;//���ݼ��������·��
	bool dircreateFlag = false;
	/*Config����ֵ*/
	QString m_depth_name;
	QString m_ir_name;
	QString m_gesture_dll;
	QString m_gesture_pbtxt;
	/*3D����*/
	int m_min_x, m_max_x;
	int m_min_y, m_max_y;
	int m_min_z, m_max_z;
	/*����*/
	bool m_init_flag = false;
	C_HandTracking *m_hand_detector = nullptr;//�ֲ������
	/*���ݼ�*/
	QString m_data_path;//��Ч���ݼ���·��
	QString m_save_path;//����������·��
	std::vector<QString> m_data_names;//��Ч���ݼ���
	std::vector<std::string> m_depth_imgs;//Depthͼ���
	std::vector<std::string> m_ir_imgs;//IRͼ���
	std::vector<cv::Point3f>pre_point3d;//ǰһ֡�㼯
	std::vector<cv::Point>pre_point2d;//ǰһ֡�㼯
	/*�������*/
	std::vector<int> m_waitkeys;//�ȴ�ʱ��
	int m_wait_key;
	bool m_start_flag = false;//�Ƿ�ʼ
	/*ϵͳ����*/
	void InitParam();//��ʼ������
	void InitWidget();//��ʼ������
	void closeEvent(QCloseEvent *event);//�رմ���
	/*�㷨���*/
	void CheckDataValid(QString file_path, std::vector<QString>& child_names);//�������ݼ���Ч��
	void Init_HandTracking();//��ʼ��HandTracking
	std::vector<cv::Point3f> Extrac_Point3D(std::vector<cv::Point> points_2D, cv::Mat depth_mat, float fx, float fy,
		float cx, float cy, int near_dis, int far_dis);//��ȡ�ֲ�3D��
	void ReadData_SaveTxt();//����
	std::vector<cv::Point3f>Normalize_Point3D(std::vector<cv::Point3f>points_3D_);//�ֲ�3D���һ��
	std::vector<cv::Point2f> Normalize_Point2D(std::vector<cv::Point>points_2D_);//�ֲ�2D���һ��
	std::vector<cv::Point3f> GetHandFeature::Normalize_Point3Ddis(std::vector<cv::Point3f>points_3D_);//�ֲ�3d��ľ����һ��
	std::vector<cv::Point2f> Normalize_Point2Ddis(std::vector<cv::Point2f>points_2D_);//�ֲ�2d��ľ����һ��
	std::vector<float>Normalize_MotionDistance3(std::vector<float>p_);//3d�����һ��
	/*�ֹ����ݼ�*/
	std::vector<cv::Point3f>getRelativeDistance3d(std::vector<cv::Point3f>current, std::vector<cv::Point3f>last);//����3d�ڵ���Ծ���
	std::vector<cv::Point2f>getRelativeDistance2d(std::vector<cv::Point>current, std::vector<cv::Point>last);//����3d�ڵ���Ծ���
	std::vector<float>getMotionDistance3D(std::vector<cv::Point3f>p_);//����3d�ڵ��ƶ�����
	
public slots:
	void hand_detect_clicked();//��ʼ
	void btn_ChooseData();//ѡ�����ݼ�
	

private:
	Ui::GetHandFeatureClass *ui;
};
