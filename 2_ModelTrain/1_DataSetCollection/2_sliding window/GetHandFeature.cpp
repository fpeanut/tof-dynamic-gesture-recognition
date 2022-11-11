#include "GetHandFeature.h"
#include "QtCommonFunction.cpp"//QT文件
#include "DynamicModuleLoader.cpp"//HandTracking文件
#include "C_HandTracking.cpp"


GetHandFeature::GetHandFeature(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::GetHandFeatureClass)
{
	ui->setupUi(this);

	m_exe_path = F_getCurRunPath();
	//初始化参数
	InitParam();
	//初始化界面
	InitWidget();
	//初始化HandTracking
	Init_HandTracking();
	//)图像保存质量
	jpg_quality.push_back(cv::IMWRITE_JPEG_QUALITY);  //选择jpeg
	jpg_quality.push_back(100); //在这个填入你要的图片质量


}

GetHandFeature::~GetHandFeature() {
	delete ui;
}

#pragma region 系统函数
//初始化参数
void GetHandFeature::InitParam() {
	//exe运行路径
	m_exe_path = F_getCurRunPath();
	//config路径
	m_config_path = m_exe_path + "/Config.INI";
	//Mediapipe模型名称
	m_gesture_dll = "hand_tracking_cpu_dll_lite.dll";
	m_gesture_pbtxt = "hand_tracking_desktop_live.pbtxt";
	//数据集
	m_depth_name = "Depth";
	m_ir_name = "IR";

	//读取Config文件
	if (!F_isFileExist(m_config_path)) {//Config文件不存在
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

//初始化界面
void GetHandFeature::InitWidget() {
	//初始化控件状态
	ui->TXefficient->setReadOnly(true);
	ui->TXpath->setReadOnly(true);
	//ui->TXrate->setReadOnly(true);
	ui->BtnStart->setEnabled(false);

}
#pragma endregion

#pragma region 控件函数
//开始
void GetHandFeature::hand_detect_clicked() {
	if (ui->BtnStart->text() == "开始") {
		ui->BtnStart->setText("结束");
		m_start_flag = true;
		//保存
		ReadData_SaveTxt();
	}
	else {
		ui->BtnStart->setText("开始");
		m_start_flag = false;
	}
}
//选择数据集
void GetHandFeature::btn_ChooseData() {
	QString temp_qstr;
	//1)选择文件夹
	temp_qstr = QFileDialog::getExistingDirectory(NULL, "选择某个手势的数据集", m_exe_path);
	if (temp_qstr.isEmpty()) {
		return;
	}
	//2)读取所有子文件夹
	m_data_names = F_getChildNames(temp_qstr);
	if (m_data_names.size() == 0) {
		QMessageBox::about(NULL, "错误", "数据集为空!");
		return;
	}
	//3)清除上次的路径参数
	ui->TXpath->clear();
	ui->TXpath->setText(temp_qstr);
	ui->TXefficient->clear();
	m_data_path = "";
	m_save_path = "";
	//4)核验数据集有效性
	m_data_path = temp_qstr;
	CheckDataValid(m_data_path, m_data_names);
	for (int i = 0; i < m_data_names.size(); i++) {
		ui->TXefficient->append(m_data_names[i]);

	}
	if (m_data_names.size() == 0) {
		QMessageBox::about(NULL, "错误", "未检测到有效动作数据集!");
		return;
	}
	ui->BtnStart->setEnabled(true);
	
}
//删除异常样本
void GetHandFeature::hand_sample_del() {
	if ((!VecDelDir.empty())&&(!VecDelLabelDir.empty())) {
		for (int i = 0; i < VecDelDir.size(); i++) {
			F_delDir(VecDelDir[i]);
			bool ok = QFile::remove(VecDelLabelDir[i]);
		}
		VecDelDir.clear();
		VecDelLabelDir.clear();
		ui->Abnoraml_Data->append("异常样本已删除");
	}
	
}
//关闭窗口
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

#pragma region 算法

//核验数据集有效性
void GetHandFeature::CheckDataValid(QString file_path, std::vector<QString>& child_names) {
	//从后往前遍历，核验每个数据集
	for (int i = child_names.size() - 1; i >= 0; i--) {
		QString temp_path = file_path + "/" + child_names[i];
		//读取当前数据集下是否有Depth和IR图像文件夹
		std::vector<QString> temp_names = F_getChildNames(temp_path);
		QString depth_name, ir_name;//depth和ir文件夹名
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
//初始化HandTracking
void GetHandFeature::Init_HandTracking() {
	m_hand_detector = new C_HandTracking(m_exe_path.toStdString(), m_gesture_dll.toStdString(),
		m_gesture_pbtxt.toStdString());//初始化HandTracking
	if (m_hand_detector->m_Init_HandTracking_OK == false) {
		QMessageBox::about(NULL, "Error", "Init Hand Tracking Failed!");
	}
	else {
		m_init_flag = true;
	}
}
//读取图像特征
void GetHandFeature::ReadData_SaveTxt() {
	//获取步长和窗口宽度
	len_sequence = ui->TxtSlipwidth->toPlainText().toInt();
	slidStep = ui->TxtSliplen->toPlainText().toInt();

	//0）定义变量
	QString data_path, depth_path, ir_path, param_path,label_path;//路径
	QString cam_id;//相机名
	float fx, fy, cx, cy;//内参
	bool level_flip = false;//翻转
	bool vertical_flip = false;
	int near_dis = 0, far_dis = 800;//深度距离

	//循环每个动作文件夹
	for (int i = 0; i < m_data_names.size(); i++) {
		if (m_start_flag == false) {
			return;
		}
		//0.1）创建单分类所有样本文件夹
		allSampledir = m_exe_path + "/SliptData";
		F_createDir(allSampledir);
		//1）路径
		data_path = m_data_path + "/" + m_data_names[i];
		depth_path = data_path + "/" + m_depth_name;
		ir_path = data_path + "/" + m_ir_name;
		param_path = data_path + "/Camera.INI";
		label_path = data_path + "/label.txt";
		//2）读取相机参数
		if (F_loadCameraParam(param_path, cam_id, vertical_flip, level_flip,
			near_dis, far_dis) == false) {
			QMessageBox::about(NULL, "错误", "读取数据集中'Camera.INI'失败!");
			continue;
		}
		//3）由相机ID读取相机内参
		QString intrinsics_path = m_exe_path + "/Camera_Intrinsics/Intrinsics_Oflim_" + cam_id + ".INI";
		if (F_loadCameraIntrinsics(intrinsics_path, fx, fy, cx, cy) == false) {
			QMessageBox::about(NULL, "错误", "未找到相机内参INI!");
			continue;
		}
		if (level_flip) {
			cx = 640 - cx;
		}
		if (vertical_flip) {
			cy = 480 - cy;
		}
		//4）遍历
		cv::Mat depth_mat = cv::Mat::zeros(640, 480, CV_8UC1);//图像
		cv::Mat ir_mat = cv::Mat::zeros(640, 480, CV_8UC1);
		cv::Mat show_depth, show_ir;
		std::vector<cv::Point> hand_points_2d;
		//4-1）读取图像数据集
		m_depth_imgs = F_loadFileImages(depth_path);
		m_ir_imgs = F_loadFileImages(ir_path);
		if (m_depth_imgs.size() == 0 || m_depth_imgs.size() != m_ir_imgs.size()) {
			QMessageBox::about(NULL, "错误", "Depth和IR数量不一致!");
			continue;
		}		
		if (m_init_flag) {
			m_hand_detector->getLandmarks(ir_mat);
		}
		//(4-2)滑动窗口分割
		int id = 0;
		for (int index = 0; index < m_depth_imgs.size(); index =index+ slidStep) {
			if (m_start_flag == false) {
				return;
			}
			int current_index = index + len_sequence;
			if (current_index > m_depth_imgs.size())
				break;
			//创建样本保存文件夹
			oneSampledir = allSampledir + "/" + m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0'));
			F_createDir(oneSampledir);
			F_createDir(oneSampledir+"/Depth");
			F_createDir(oneSampledir + "/IR");
			bool ok = QFile::copy(param_path, oneSampledir+"/Camera.INI");
			ok = QFile::copy(label_path, allSampledir + "/"+ m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0'))+".txt");
			//数据集分割
			int point_num = 0;
			for (int start_index = index; start_index < index + len_sequence; start_index++) {
				//获取当前的时间
				QDateTime current_date_time = QDateTime::currentDateTime();
				QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");
				//4-3）读取保存Depth和IR			
				depth_mat = cv::imread(m_depth_imgs[start_index], 0);
				ir_mat = cv::imread(m_ir_imgs[start_index], 0);

				cv::imwrite(oneSampledir.toStdString() + "/Depth/"+ current_date.toStdString()+".jpg", depth_mat, jpg_quality);
				cv::imwrite(oneSampledir.toStdString() + "/IR/" + current_date.toStdString() + ".jpg", ir_mat, jpg_quality);
				depth_mat.copyTo(show_depth);
				ir_mat.copyTo(show_ir);

				//4-4）检测IR图像手部点
				if (m_init_flag) {
					m_hand_detector->getLandmarks(ir_mat);
					hand_points_2d = m_hand_detector->landmarks;
					if (hand_points_2d.size() != 0)
						point_num++;

				}
				//显示
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
			//打印异常样本
			if (float(point_num) / len_sequence < ui->Txtrate->toPlainText().toFloat()) {
				qDebug() << float(point_num) / len_sequence;
				ui->Abnoraml_Data->append(m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0')));
				VecDelDir.push_back(oneSampledir);
				VecDelLabelDir.push_back(allSampledir + "/" + m_data_names[i] + "_" + QString("%1").arg(id, 4, 10, QLatin1Char('0')) + ".txt");
			}
			id++;
		}		
	}
	ui->BtnStart->setText("开始");
	m_start_flag = false;
}

#pragma endregion
