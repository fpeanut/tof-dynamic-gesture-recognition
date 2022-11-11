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
	//)ͼ�񱣴�����
	jpg_quality.push_back(cv::IMWRITE_JPEG_QUALITY);  //ѡ��jpeg
	jpg_quality.push_back(100); //�����������Ҫ��ͼƬ����


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

	//��ȡConfig�ļ�
	if (!F_isFileExist(m_config_path)) {//Config�ļ�������
		F_saveQstring_INI(m_config_path, "HandTracking", "dll_name", m_gesture_dll);
		F_saveQstring_INI(m_config_path, "HandTracking", "pbtxt_name", m_gesture_pbtxt);
		F_saveQstring_INI(m_config_path, "DataBase", "depth_name", m_depth_name);
		F_saveQstring_INI(m_config_path, "DataBase", "ir_name", m_ir_name);

	}
	else {
		F_loadQstring_INI(m_config_path, "HandTracking", "dll_name", m_gesture_dll);
		F_loadQstring_INI(m_config_path, "HandTracking", "pbtxt_name", m_gesture_pbtxt);
		F_loadQstring_INI(m_config_path, "DataBase", "depth_name", m_depth_name);
		F_loadQstring_INI(m_config_path, "DataBase", "ir_name", m_ir_name);
	}	
}

//��ʼ������
void GetHandFeature::InitWidget() {
	//��ʼ���ؼ�״̬
	ui->TXefficient->setReadOnly(true);
	ui->TXpath->setReadOnly(true);
	//ui->TXrate->setReadOnly(true);
	ui->BtnStart->setEnabled(false);

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
	ui->BtnStart->setEnabled(true);
	
}
//ɾ���쳣����
void GetHandFeature::hand_sample_del() {
	if ((!VecDelDir.empty())&&(!VecDelLabelDir.empty())) {
		for (int i = 0; i < VecDelDir.size(); i++) {
			F_delDir(VecDelDir[i]);
			bool ok = QFile::remove(VecDelLabelDir[i]);
		}
		VecDelDir.clear();
		VecDelLabelDir.clear();
		ui->Abnoraml_Data->append("�쳣������ɾ��");
	}
	
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
	if (m_hand_detector->m_Init_HandTracking_OK == false) {
		QMessageBox::about(NULL, "Error", "Init Hand Tracking Failed!");
	}
	else {
		m_init_flag = true;
	}
}
//��ȡͼ������
void GetHandFeature::ReadData_SaveTxt() {
	//��ȡ�����ʹ��ڿ��
	len_sequence = ui->TxtSlipwidth->toPlainText().toInt();
	slidStep = ui->TxtSliplen->toPlainText().toInt();

	//0���������
	QString data_path, depth_path, ir_path, param_path,label_path;//·��
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
		//0.1���������������������ļ���
		allSampledir = m_exe_path + "/SliptData";
		F_createDir(allSampledir);
		//1��·��
		data_path = m_data_path + "/" + m_data_names[i];
		depth_path = data_path + "/" + m_depth_name;
		ir_path = data_path + "/" + m_ir_name;
		param_path = data_path + "/Camera.INI";
		label_path = data_path + "/label.txt";
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
		//4-1����ȡͼ�����ݼ�
		m_depth_imgs = F_loadFileImages(depth_path);
		m_ir_imgs = F_loadFileImages(ir_path);
		if (m_depth_imgs.size() == 0 || m_depth_imgs.size() != m_ir_imgs.size()) {
			QMessageBox::about(NULL, "����", "Depth��IR������һ��!");
			continue;
		}		
		if (m_init_flag) {
			m_hand_detector->getLandmarks(ir_mat);
		}
		//(4-2)�������ڷָ�
		int id = 0;
		for (int index = 0; index < m_depth_imgs.size(); index =index+ slidStep) {
			if (m_start_flag == false) {
				return;
			}
			int current_index = index + len_sequence;
			if (current_index > m_depth_imgs.size())
				break;
			//�������������ļ���
			oneSampledir = allSampledir + "/" + m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0'));
			F_createDir(oneSampledir);
			F_createDir(oneSampledir+"/Depth");
			F_createDir(oneSampledir + "/IR");
			bool ok = QFile::copy(param_path, oneSampledir+"/Camera.INI");
			ok = QFile::copy(label_path, allSampledir + "/"+ m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0'))+".txt");
			//���ݼ��ָ�
			int point_num = 0;
			for (int start_index = index; start_index < index + len_sequence; start_index++) {
				//��ȡ��ǰ��ʱ��
				QDateTime current_date_time = QDateTime::currentDateTime();
				QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");
				//4-3����ȡ����Depth��IR			
				depth_mat = cv::imread(m_depth_imgs[start_index], 0);
				ir_mat = cv::imread(m_ir_imgs[start_index], 0);

				cv::imwrite(oneSampledir.toStdString() + "/Depth/"+ current_date.toStdString()+".jpg", depth_mat, jpg_quality);
				cv::imwrite(oneSampledir.toStdString() + "/IR/" + current_date.toStdString() + ".jpg", ir_mat, jpg_quality);
				depth_mat.copyTo(show_depth);
				ir_mat.copyTo(show_ir);

				//4-4�����IRͼ���ֲ���
				if (m_init_flag) {
					m_hand_detector->getLandmarks(ir_mat);
					hand_points_2d = m_hand_detector->landmarks;
					if (hand_points_2d.size() != 0)
						point_num++;

				}
				//��ʾ
				F_drawHand(show_depth, hand_points_2d);
				F_drawHand(show_ir, hand_points_2d);
				F_showMat(show_depth, ui->DepthLabel);
				F_showMat(show_ir, ui->IRLabel);
				cv::waitKey(ui->Txtwait->toPlainText().toInt());
				//
				if (start_index == 60) {
					//qDebug() << index << "," << index + len_sequence;
					VecDelDir.push_back(oneSampledir);
					VecDelLabelDir.push_back(allSampledir + "/" + m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0')) + ".txt");
				}
			}
			//��ӡ�쳣����
			if (float(point_num) / len_sequence < ui->Txtrate->toPlainText().toFloat()) {
				qDebug() << float(point_num) / len_sequence;
				ui->Abnoraml_Data->append(m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0')));
				VecDelDir.push_back(oneSampledir);
				VecDelLabelDir.push_back(allSampledir + "/" + m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0')) + ".txt");
			}
			id++;
		}		
	}
	ui->BtnStart->setText("��ʼ");
	m_start_flag = false;
}

#pragma endregion
