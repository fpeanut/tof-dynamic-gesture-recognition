#include "GetHandFeature.h"
#include "QtCommonFunction.cpp"//QT�ļ�
#include "DynamicModuleLoader.cpp"//HandTracking�ļ�
#include "C_HandTracking.cpp"


GetHandFeature::GetHandFeature(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::GetHandFeatureClass)
{
	ui->setupUi(this);

	m_exe_path = F_getCurRunPath();
	//��ʼ������
	InitParam();
	//��ʼ������
	InitWidget();
	//��ʼ��HandTracking
	Init_HandTracking();
}

GetHandFeature::~GetHandFeature() {
	delete ui;
}

#pragma region ϵͳ����
//��ʼ������
void GetHandFeature::InitParam() {
	//exe����·��
	m_exe_path = F_getCurRunPath();
	//config·��
	m_config_path = m_exe_path + "/Config.INI";
	//Mediapipeģ������
	m_gesture_dll = "hand_tracking_cpu_dll_lite.dll";
	m_gesture_pbtxt = "hand_tracking_desktop_live.pbtxt";
	//���ݼ�
	m_depth_name = "Depth";
	m_ir_name = "IR";
	//����ROI
	m_min_x = -300;
	m_max_x = 300;
	m_min_y = -200;
	m_max_y = 200;
	m_min_z = 0;
	m_max_z = 800;
	//��ȡConfig�ļ�
	if (!F_isFileExist(m_config_path)) {//Config�ļ�������
		F_saveQstring_INI(m_config_path, "HandTracking", "dll_name", m_gesture_dll);
		F_saveQstring_INI(m_config_path, "HandTracking", "pbtxt_name", m_gesture_pbtxt);
		F_saveQstring_INI(m_config_path, "DataBase", "depth_name", m_depth_name);
		F_saveQstring_INI(m_config_path, "DataBase", "ir_name", m_ir_name);
		F_saveInt_INI(m_config_path, "GestureRoi", "min_x", m_min_x);
		F_saveInt_INI(m_config_path, "GestureRoi", "max_x", m_max_x);
		F_saveInt_INI(m_config_path, "GestureRoi", "min_y", m_min_y);
		F_saveInt_INI(m_config_path, "GestureRoi", "max_y", m_max_y);
		F_saveInt_INI(m_config_path, "GestureRoi", "min_z", m_min_z);
		F_saveInt_INI(m_config_path, "GestureRoi", "max_z", m_max_z);
	}
	else {
		F_loadQstring_INI(m_config_path, "HandTracking", "dll_name", m_gesture_dll);
		F_loadQstring_INI(m_config_path, "HandTracking", "pbtxt_name", m_gesture_pbtxt);
		F_loadQstring_INI(m_config_path, "DataBase", "depth_name", m_depth_name);
		F_loadQstring_INI(m_config_path, "DataBase", "ir_name", m_ir_name);
		F_loadInt_INI(m_config_path, "GestureRoi", "min_x", m_min_x);
		F_loadInt_INI(m_config_path, "GestureRoi", "max_x", m_max_x);
		F_loadInt_INI(m_config_path, "GestureRoi", "min_y", m_min_y);
		F_loadInt_INI(m_config_path, "GestureRoi", "max_y", m_max_y);
		F_loadInt_INI(m_config_path, "GestureRoi", "min_z", m_min_z);
		F_loadInt_INI(m_config_path, "GestureRoi", "max_z", m_max_z);
	}
	//�������
	m_waitkeys = { 1, 10, 40, 500 };
}

//��ʼ������
void GetHandFeature::InitWidget() {
	//��ʼ���ؼ�״̬
	ui->TXminx->setReadOnly(true);
	ui->TXminy->setReadOnly(true);
	ui->TXminz->setReadOnly(true);
	ui->TXmaxx->setReadOnly(true);
	ui->TXmaxy->setReadOnly(true);
	ui->TXmaxz->setReadOnly(true);
	ui->TXefficient->setReadOnly(true);
	ui->TXpath->setReadOnly(true);
	//ui->TXrate->setReadOnly(true);
	ui->BtnStart->setEnabled(false);
	//������ʾ
	ui->TXminx->setText(QString::number(m_min_x));
	ui->TXminy->setText(QString::number(m_max_x));
	ui->TXminz->setText(QString::number(m_min_y));
	ui->TXmaxx->setText(QString::number(m_max_y));
	ui->TXmaxy->setText(QString::number(m_min_z));
	ui->TXmaxz->setText(QString::number(m_max_z));
}
#pragma endregion

#pragma region �ؼ�����
//��ʼ
void GetHandFeature::hand_detect_clicked() {
	if (ui->BtnStart->text() == "��ʼ") {
		ui->BtnStart->setText("����");
		m_start_flag = true;
		//����
		ReadData_SaveTxt();
	}
	else {
		ui->BtnStart->setText("��ʼ");
		m_start_flag = false;
	}
}
//ѡ�����ݼ�
void GetHandFeature::btn_ChooseData() {
	QString temp_qstr;
	//1)ѡ���ļ���
	temp_qstr = QFileDialog::getExistingDirectory(NULL, "ѡ��ĳ�����Ƶ����ݼ�", m_exe_path);
	if (temp_qstr.isEmpty()) {
		return;
	}
	//2)��ȡ�������ļ���
	m_data_names = F_getChildNames(temp_qstr);
	if (m_data_names.size() == 0) {
		QMessageBox::about(NULL, "����", "���ݼ�Ϊ��!");
		return;
	}
	//3)����ϴε�·������
	ui->TXpath->clear();
	ui->TXpath->setText(temp_qstr);
	ui->TXefficient->clear();
	ui->TXSave->clear();
	m_data_path = "";
	m_save_path = "";
	//4)�������ݼ���Ч��
	m_data_path = temp_qstr;
	CheckDataValid(m_data_path, m_data_names);
	for (int i = 0; i < m_data_names.size(); i++) {
		ui->TXefficient->append(m_data_names[i]);

	}
	if (m_data_names.size() == 0) {
		QMessageBox::about(NULL, "����", "δ��⵽��Ч�������ݼ�!");
		return;
	}
	//5)���ñ���·��
	m_save_path = m_data_path + "/Save_Txt";
	ui->TXSave->setText(m_save_path);
	ui->BtnStart->setEnabled(true);
	//6)����ѵ��������ļ���
	m_trainPath_label = m_exe_path + "/DataSet/label";
	m_trainPath_points2d = m_exe_path + "/DataSet/points/2d";
	m_trainPath_points3d= m_exe_path + "/DataSet/points/3d";
	m_trainPath_points3dn= m_exe_path + "/DataSet/points/3dNormal";
	m_trainPath_MixedFeatures = m_exe_path + "/DataSet/points/MixedFeatures";
	dircreateFlag=F_createDir(m_trainPath_label);
	if (!dircreateFlag)
		cout << "�ļ���label��������" << endl;
	dircreateFlag = F_createDir(m_trainPath_points2d);
	if (!dircreateFlag)
		cout << "�ļ���points2d��������" << endl;
	dircreateFlag = F_createDir(m_trainPath_points3d);
	if (!dircreateFlag)
		cout << "�ļ���points3d��������" << endl;
	dircreateFlag = F_createDir(m_trainPath_points3dn);
	if (!dircreateFlag)
		cout << "�ļ���points3dNormal��������" << endl;
	dircreateFlag = F_createDir(m_trainPath_MixedFeatures);
	if (!dircreateFlag)
		cout << "�ļ���MixedFeatures��������" << endl;
}
//�رմ���
void GetHandFeature::closeEvent(QCloseEvent *event) {
	Sleep(100);
	if (m_init_flag) {
		m_hand_detector->~C_HandTracking();
	}
	Sleep(100);
	cv::destroyAllWindows();
	QApplication::quit();
}
#pragma endregion

#pragma region �㷨

//�������ݼ���Ч��
void GetHandFeature::CheckDataValid(QString file_path, std::vector<QString>& child_names) {
	//�Ӻ���ǰ����������ÿ�����ݼ�
	for (int i = child_names.size() - 1; i >= 0; i--) {
		QString temp_path = file_path + "/" + child_names[i];
		//��ȡ��ǰ���ݼ����Ƿ���Depth��IRͼ���ļ���
		std::vector<QString> temp_names = F_getChildNames(temp_path);
		QString depth_name, ir_name;//depth��ir�ļ�����
		for (int i = 0; i < temp_names.size(); ++i)
		{
			QString temp_name = temp_names.at(i);
			if (temp_name == m_depth_name) {
				depth_name = temp_name;
			}
			else if (temp_name == m_ir_name) {
				ir_name = temp_name;
			}
		}
		if (depth_name.isEmpty() || ir_name.isEmpty()) {
			child_names.erase(child_names.begin() + i);
		}
	}
}
//��ʼ��HandTracking
void GetHandFeature::Init_HandTracking() {
	m_hand_detector = new C_HandTracking(m_exe_path.toStdString(), m_gesture_dll.toStdString(),
		m_gesture_pbtxt.toStdString());//��ʼ��HandTracking
	qDebug() << m_exe_path;
	if (m_hand_detector->m_Init_HandTracking_OK == false) {
		QMessageBox::about(NULL, "Error", "Init Hand Tracking Failed!");
	}
	else {
		m_init_flag = true;
	}
}
//��ȡ�ֲ�3D��
std::vector<cv::Point3f> GetHandFeature::Extrac_Point3D(std::vector<cv::Point> points_2D, cv::Mat depth_mat, float fx, float fy,
	float cx, float cy, int near_dis, int far_dis) {
	std::vector<cv::Point3f> points_3D;
	for (int i = 0; i < points_2D.size(); i++) {
		cv::Point2i hand_point = points_2D[i];//ͼ��2D���ص�
		int gray_value = 0;
		if (hand_point.x >= 0 && hand_point.x < 640 && hand_point.y >= 0 && hand_point.y < 480) {//2D����ͼ���ڲ�
			gray_value = depth_mat.at<uchar>(hand_point.y, hand_point.x);//��ûҶ�ֵ
		}
		float z_value = gray_value / 255.0 * (far_dis - near_dis) + near_dis;//�������ֵ
		float x_value = (hand_point.x - cx) / fx * z_value;
		float y_value = (hand_point.y - cy) / fy * z_value;
		points_3D.push_back(cv::Point3f(x_value, y_value, z_value));
	}
	return points_3D;
}
//3d���һ��
std::vector<cv::Point3f> GetHandFeature::Normalize_Point3D(std::vector<cv::Point3f>points_3D_) {
	std::vector<cv::Point3f> tmp_point;
	if (points_3D_.size() > 0) {
		//��һ��
		float tmpx = 0;
		float tmpy = 0;
		float tmpz = 0;
		for (int m = 0; m < 21; m++) {
			if (points_3D_[m].x> m_min_x&&points_3D_[m].x<m_max_x&&points_3D_[m].y> m_min_y&&points_3D_[m].y<m_max_y&&points_3D_[m].z> m_min_z&&points_3D_[m].z<m_max_z) {
				tmpx = (points_3D_[m].x - m_min_x) / (m_max_x - m_min_x);
				tmpy = (points_3D_[m].y - m_min_y) / (m_max_y - m_min_y);
				tmpz = (points_3D_[m].z - m_min_z) / (m_max_z - m_min_z);
			}
			
			tmp_point.push_back(cv::Point3f(tmpx, tmpy, tmpz));
		}
	}
	else
	{
		for (int i = 0; i < 21; i++) {
			tmp_point.push_back(cv::Point3f(0, 0, 0));
		}
	}
	
	return tmp_point;
}
//2d���һ��
std::vector<cv::Point2f> GetHandFeature::Normalize_Point2D(std::vector<cv::Point>points_2D_) {
	std::vector<cv::Point2f> tmp_point;
	if (points_2D_.size() > 0) {
		//��һ��
		float tmpx = 0;
		float tmpy = 0;
		for (int m = 0; m < 21; m++) {
			if (points_2D_[m].x > 0 && points_2D_[m].x < 640 && points_2D_[m].y>0 && points_2D_[m].y < 480) {
				tmpx = float(points_2D_[m].x - 0) / (640 - 0);
				tmpy = float(points_2D_[m].y - 0) / (480 - 0);
			}		
			tmp_point.push_back(cv::Point2f(tmpx, tmpy));
			/*std::cout << tmp_point[m].x << "," << tmp_point[m].y << endl;*/
		}
	}
	else
	{
		for (int i = 0; i < 21; i++) {
			tmp_point.push_back(cv::Point2f(0, 0));
		}
	}
	return tmp_point;
}
//3d������һ��
std::vector<cv::Point3f> GetHandFeature::Normalize_Point3Ddis(std::vector<cv::Point3f>points_3D_) {
	std::vector<cv::Point3f> tmp_point;
	if (points_3D_.size() > 0) {
		//��һ��
		float tmpx = 0;
		float tmpy = 0;
		float tmpz = 0;
		for (int m = 0; m < 21; m++) {
			tmpx = (points_3D_[m].x) / (m_max_x - m_min_x);
			tmpy = (points_3D_[m].y) / (m_max_y - m_min_y);
			tmpz = (points_3D_[m].z) / (m_max_z - m_min_z);

			tmp_point.push_back(cv::Point3f(tmpx, tmpy, tmpz));
		}
	}
	else
	{
		for (int i = 0; i < 21; i++) {
			tmp_point.push_back(cv::Point3f(0, 0, 0));
		}
	}

	return tmp_point;
}
//2d������һ��
std::vector<cv::Point2f> GetHandFeature::Normalize_Point2Ddis(std::vector<cv::Point2f>points_2D_) {
	std::vector<cv::Point2f> tmp_point;
	if (points_2D_.size() > 0) {
		//��һ��
		float tmpx = 0;
		float tmpy = 0;
		for (int m = 0; m < 21; m++) {
			tmpx = float(points_2D_[m].x - 0) / (640 - 0);
			tmpy = float(points_2D_[m].y - 0) / (480 - 0);
			tmp_point.push_back(cv::Point(tmpx, tmpy));
		}
	}
	else
	{
		for (int i = 0; i < 21; i++) {
			tmp_point.push_back(cv::Point(0, 0));
		}
	}
	return tmp_point;
}
//3d�����һ��
std::vector<float>GetHandFeature::Normalize_MotionDistance3(std::vector<float>p_) {
	std::vector<float>temp;
	for (int i = 0; i < p_.size(); i++) {
		float tmp = 0.0;
		tmp = p_[i] / ((m_max_x - m_min_x)*(m_max_x - m_min_x) + (m_max_y - m_min_y)*(m_max_y - m_min_y) + (m_max_z - m_min_z)*(m_max_z - m_min_z));
		temp.push_back(tmp);
	}
	return temp;
}
//��ȡͼ������
void GetHandFeature::ReadData_SaveTxt() {
	//0���������
	QString data_path, depth_path, ir_path, param_path;//·��
	QString cam_id;//�����
	float fx, fy, cx, cy;//�ڲ�
	bool level_flip = false;//��ת
	bool vertical_flip = false;
	int near_dis = 0, far_dis = 800;//��Ⱦ���

	//ѭ��ÿ�������ļ���
	for (int i = 0; i < m_data_names.size(); i++) {
		if (m_start_flag == false) {
			return;
		}
		//0)����ѵ����point2d��3d��3dnormal�ļ���
		QString PointsCurrent2d = m_trainPath_points2d + "/" + m_data_names[i];
		QString PointsCurrent3d = m_trainPath_points3d + "/" + m_data_names[i];
		QString PointsCurrent3dn = m_trainPath_points3dn + "/" + m_data_names[i];
		QString CurrentMixedFeatures = m_trainPath_MixedFeatures + "/" + m_data_names[i];
		dircreateFlag = F_createDir(PointsCurrent2d);
		if (!dircreateFlag)
			cout << "�ļ���PointsCurrent2d��������" << endl;
		dircreateFlag = F_createDir(PointsCurrent3d);
		if (!dircreateFlag)
			cout << "�ļ���PointsCurrent3d��������" << endl;
		dircreateFlag = F_createDir(PointsCurrent3dn);
		if (!dircreateFlag)
			cout << "�ļ���PointsCurrent3dn��������" << endl;
		dircreateFlag = F_createDir(CurrentMixedFeatures);
		if (!dircreateFlag)
			cout << "�ļ���CurrentMixedFeatures��������" << endl;
		//1��·��
		data_path = m_data_path + "/" + m_data_names[i];
		depth_path = data_path + "/" + m_depth_name;
		ir_path = data_path + "/" + m_ir_name;
		param_path = data_path + "/Camera.INI";
		//2����ȡ�������
		if (F_loadCameraParam(param_path, cam_id, vertical_flip, level_flip,
			near_dis, far_dis) == false) {
			QMessageBox::about(NULL, "����", "��ȡ���ݼ���'Camera.INI'ʧ��!");
			continue;
		}
		//3�������ID��ȡ����ڲ�
		QString intrinsics_path = m_exe_path + "/Camera_Intrinsics/Intrinsics_Oflim_" + cam_id + ".INI";
		if (F_loadCameraIntrinsics(intrinsics_path, fx, fy, cx, cy) == false) {
			QMessageBox::about(NULL, "����", "δ�ҵ�����ڲ�INI!");
			continue;
		}
		if (level_flip) {
			cx = 640 - cx;
		}
		if (vertical_flip) {
			cy = 480 - cy;
		}
		//4������
		cv::Mat depth_mat = cv::Mat::zeros(640, 480, CV_8UC1);//ͼ��
		cv::Mat ir_mat = cv::Mat::zeros(640, 480, CV_8UC1);
		cv::Mat show_depth, show_ir;
		std::vector<cv::Point> hand_points_2d;
		std::vector<cv::Point3f> hand_points_3d;
		std::vector<cv::Point3f> hand_points_3dn;
		//4-1����ȡͼ�����ݼ�
		m_depth_imgs = F_loadFileImages(depth_path);
		m_ir_imgs = F_loadFileImages(ir_path);
		if (m_depth_imgs.size() == 0 || m_depth_imgs.size() != m_ir_imgs.size()) {
			QMessageBox::about(NULL, "����", "Depth��IR������һ��!");
			continue;
		}
		//4-2������ͼ��ȷ����һ֡�Ǽ��
		if (m_init_flag) {
			m_hand_detector->getLandmarks(ir_mat);
		}
		for (int j = 0; j < m_depth_imgs.size(); j++) {
			
			if (m_start_flag == false) {
				return;
			}
			hand_points_2d.clear();
			hand_points_3d.clear();
			hand_points_3dn.clear();
			//4-2.5)����pointsѵ�����ļ���
			QFileInfo fileInfo(QString::fromStdString(m_depth_imgs[j]));
			F_createFile(PointsCurrent2d + "/" + fileInfo.baseName()+".txt");//��ȡͼ����������׺
			F_createFile(PointsCurrent3d + "/" + fileInfo.baseName() + ".txt");
			F_createFile(PointsCurrent3dn + "/" + fileInfo.baseName() + ".txt");
			F_createFile(CurrentMixedFeatures + "/" + fileInfo.baseName() + ".txt");
	
			//4-3����ȡDepth��IR			
			depth_mat = cv::imread(m_depth_imgs[j], 0);			
			ir_mat = cv::imread(m_ir_imgs[j], 0);			
			depth_mat.copyTo(show_depth);
			ir_mat.copyTo(show_ir);
			//4-4�����IRͼ���ֲ���
			if (m_init_flag) {
				m_hand_detector->getLandmarks(ir_mat);
				hand_points_2d = m_hand_detector->landmarks;
					
			}	
			if (hand_points_2d.size() == 0) {
				for (int point = 0; point < 21; point++) {
					hand_points_2d.push_back(cv::Point2f(0, 0));
				}
			}
			//4-5������3D��
			hand_points_3d = Extrac_Point3D(hand_points_2d, depth_mat, fx, fy, cx, cy, near_dis, far_dis);
			//4-6������roi��һ��3d
			hand_points_3dn = Normalize_Point3D(hand_points_3d);	
			//4-7) ����3d�ڵ���Ծ���
			std::vector<cv::Point3f>detaDistance3d;
			if (j == 0) {
				for (int index = 0; index < hand_points_3d.size(); index++) {
					detaDistance3d.push_back(cv::Point3f(0, 0, 0));
				}
			}
			else
			{
				detaDistance3d = getRelativeDistance3d(hand_points_3d, pre_point3d);
			}
			pre_point3d = hand_points_3d;
			//4-8) ����2d�ڵ���Ծ���
			std::vector<cv::Point2f>detaDistance2d;
			if (j == 0) {
				for (int index = 0; index < hand_points_3d.size(); index++) {
					detaDistance2d.push_back(cv::Point2f(0, 0));
				}
			}
			else
			{
				detaDistance2d = getRelativeDistance2d(hand_points_2d, pre_point2d);
			}
			pre_point2d = hand_points_2d;
			//4-9����һ��2D dis ,3D dis,2Dpoints
			std::vector<cv::Point3f> normal_3Ddis;
			std::vector<cv::Point2f> normal_2Ddis;
			std::vector<cv::Point2f> normal_2D;
			normal_3Ddis = Normalize_Point3Ddis(detaDistance3d);
			normal_2Ddis = Normalize_Point2Ddis(detaDistance2d);
			normal_2D= Normalize_Point2D(hand_points_2d);
			//4-10)����3d���ƶ�����
			std::vector<float>motionDis3D;
			motionDis3D = getMotionDistance3D(detaDistance3d);
			//4-11)3d���ƶ������һ��
			std::vector<float>motionDis3D_nromal;
			motionDis3D_nromal = Normalize_MotionDistance3(motionDis3D);
			//4-12)����2d��3d��3dn��
			QFile file, file1, file2,file3;
			file.setFileName(PointsCurrent2d + "/" + fileInfo.baseName() + ".txt");
			file1.setFileName(PointsCurrent3d + "/" + fileInfo.baseName() + ".txt");
			file2.setFileName(PointsCurrent3dn + "/" + fileInfo.baseName() + ".txt");
			file3.setFileName(CurrentMixedFeatures + "/" + fileInfo.baseName() + ".txt");
			if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				for (int i = 0; i < 21; i++) {
					QTextStream stream(&file);
					stream << QString::number(hand_points_2d[i].x) + "," + QString::number(hand_points_2d[i].y) << "\n";
				}
				file.close();
			}	//2d
			if (file1.open(QIODevice::WriteOnly | QIODevice::Text)) {
				for (int i = 0; i < hand_points_3d.size(); i++) {
					QTextStream stream(&file1);
					stream << QString::number(hand_points_3d[i].x) + "," + QString::number(hand_points_3d[i].y) + "," + QString::number(hand_points_3d[i].z) <<"\n";
				}
				file1.close();
			}//3d
			if (file2.open(QIODevice::WriteOnly | QIODevice::Text)) {
				for (int i = 0; i < hand_points_3dn.size(); i++) {
					QTextStream stream(&file2);
					stream << QString::number(hand_points_3dn[i].x) + "," + QString::number(hand_points_3dn[i].y) + "," + QString::number(hand_points_3dn[i].z) << "\n";
				}
				file2.close();
			}//3dn
			if (file3.open(QIODevice::WriteOnly | QIODevice::Text)) {
				for (int i = 0; i < hand_points_3d.size(); i++) {
					QTextStream stream(&file3);
					/*stream << QString::number(normal_2D[i].x) + "," + QString::number(normal_2D[i].y)
						+ "," + QString::number(hand_points_3dn[i].x) + "," + QString::number(hand_points_3dn[i].y) + "," + QString::number(hand_points_3dn[i].z)
						+ "," + QString::number(normal_2Ddis[i].x) + "," + QString::number(normal_2Ddis[i].y)
						+ "," + QString::number(normal_3Ddis[i].x) + "," + QString::number(normal_3Ddis[i].y) + "," + QString::number(normal_3Ddis[i].z) << "\n";*/
					stream << QString::number(hand_points_3dn[i].x) + "," + QString::number(hand_points_3dn[i].y) + "," + QString::number(hand_points_3dn[i].z)
						+ "," + QString::number(normal_3Ddis[i].x) + "," + QString::number(normal_3Ddis[i].y) + "," + QString::number(normal_3Ddis[i].z) + "," + QString::number(motionDis3D_nromal[i])<< "\n";
				}
				file3.close();
			}//2d 3d 2dis 3dis
			//����ʾ���
			F_drawHand(show_depth, hand_points_2d);
			F_drawHand(show_ir, hand_points_2d);
			F_showMat(show_depth, ui->DepthLabel);

			F_showMat(show_ir, ui->IRLabel);
			cv::waitKey(ui->TXrate->toPlainText().toInt());
		}
	}
	ui->BtnStart->setText("��ʼ");
	m_start_flag = false;
}

#pragma endregion
//����3d�ڵ���Ծ���
std::vector<cv::Point3f>GetHandFeature::getRelativeDistance3d(std::vector<cv::Point3f>current, std::vector<cv::Point3f>last) {
	std::vector<cv::Point3f>temp;
	for (int i = 0; i < current.size(); i++) {
		cv::Point3f ptmp;
		ptmp.x = current[i].x - last[i].x;
		ptmp.y = current[i].y - last[i].y;
		ptmp.z = current[i].z - last[i].z;
		temp.push_back(ptmp);
	}
	return temp;
}
//����2d�ڵ���Ծ���
std::vector<cv::Point2f>GetHandFeature::getRelativeDistance2d(std::vector<cv::Point>current, std::vector<cv::Point>last) {
	std::vector<cv::Point2f>temp;
	for (int i = 0; i < current.size(); i++) {
		cv::Point2f ptmp;
		ptmp.x = current[i].x - last[i].x;
		ptmp.y = current[i].y - last[i].y;		
		temp.push_back(ptmp);
	}
	return temp;
}
//����3d�ڵ��ƶ�����
std::vector<float>GetHandFeature::getMotionDistance3D(std::vector<cv::Point3f>p_) {
	std::vector<float>dis;
	for (int i = 0; i < p_.size(); i++) {
		float temp;
		temp = sqrtf(p_[i].x*p_[i].x + p_[i].y*p_[i].y + p_[i].z*p_[i].z);
		dis.push_back(temp);
	}
	return dis;
}