#include "GrapImgTof.h"
#include "QtCommonFunction.cpp"//QT文件
#include "OflimListener.cpp"//Oflim相机Tof类

GrapImgTof::GrapImgTof(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::GrapImgTofClass)
{
	ui->setupUi(this);
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);//固定窗口大小，无法改变
	//定时器
	time_.setHMS(0, 0, 0, 0);//设置初始值   
	timer_ = new QTimer(this);//创建一个定时器   
	connect(timer_, SIGNAL(timeout()), this, SLOT(update()));

	
	//a)界面参数初始化
	initUiParam();
	//b)获取图像保存地址
	GetSavePath();

	//)图像保存质量
	jpg_quality.push_back(cv::IMWRITE_JPEG_QUALITY);  //选择jpeg
	jpg_quality.push_back(100); //在这个填入你要的图片质量
}

//(a)界面参数初始化
void GrapImgTof::initUiParam() {
	//exe运行路径
	m_exe_path = F_getCurRunPath();
	//config路径
	m_config_path = m_exe_path + "/config.ini";
	//读取config文件
	F_loadQstring_INI(m_config_path, "HandGesture","handGT", m_hand_items);
	QStringList m_list = m_hand_items.split("/");
	qDebug() << m_list[0];
	//combox菜单设置
	ui->comboBoxFlip->addItem("-");
	ui->comboBoxFlip->addItem("水平翻转");
	ui->comboBoxFlip->addItem("垂直翻转");
	ui->comboBoxHand->addItem("-");
	for (int i = 0; i < m_list.size(); i++) {
		ui->comboBoxHand->addItem(m_list[i]);
	}
	ui->comboBoxImgs->addItem("jpg");
	ui->comboBoxImgs->addItem("bmp");
	
	//相机
	for (int i = 0; i < m_tof_num; i++) {
		QString temp = QString::fromStdString(m_oflim_listener->cameraListenerVec[i].tof_cameraID.toStdString());
		ui->textEditPrint->append("连接相机："+temp);
	}
}
//(b)获取图像保存地址
QString GrapImgTof::GetSavePath() {
	QString currentPath = QDir::currentPath();//获取当前路径
	dataTime = QDateTime::currentDateTime();//获取当前时间
	QString timeToday;
	timeToday = dataTime.toString("yyyy_MM_dd");
	QDir dir, dir_next0;
	QString firstPath = currentPath + "/SaveImage/";
	QString secondtPath;
	//创建SaveImage
	if (!dir.exists(firstPath)) {
		dir.mkdir(firstPath);
	}
	else
	{
		qDebug() << "SaveImage文件夹已创建";
	}
	//创建SaveImage/年月日
	if (dir.exists(firstPath)) {
		secondtPath = firstPath + timeToday + "/";
		if (!dir_next0.exists(secondtPath)) {
			dir_next0.mkdir(secondtPath);
		}
		else
		{
			qDebug() << "SaveImage/" + timeToday + "文件夹已创建";
		}
	}
	//创建SaveImage/年月日/种类
	if (dir_next0.exists(secondtPath)) {
		QDir dir_next1;
		QString thirthPath = secondtPath + ui->textEditDir->toPlainText() + "/";
		if (!dir_next1.exists(thirthPath)) {
			dir_next1.mkdir(thirthPath);
			return thirthPath;
		}
		else
		{
			qDebug() << "SaveImage/" + timeToday + "/" + ui->textEditPrint->toPlainText() + "文件夹已创建";
			return thirthPath;
		}
	}
}
//(c)显示Mat图像
void GrapImgTof::ShowMat(cv::Mat image, QLabel *show_image)
{
	//图片的显示主要问题还是在于QImage与Mat格式的不同，其中QImage是RGB排列，而Mat 是BGR，同时两者格式包含的内容页不同，因此需要经过转换才能够显示。
	cv::Mat rgb_image;
	QImage q_image;
	if (image.channels() == 3)//RGB Img
	{
		cv::cvtColor(image, image, /*CV_BGR2RGB*/4);//颜色空间转换
		q_image = QImage((const uchar*)(image.data), image.cols, image.rows, image.cols * image.channels(), QImage::Format_RGB888);
	}
	else//Gray Img
	{
		//cv::cvtColor(temp, Rgb,CV_GRAY2RGB);//颜色空间转换
		//Img = QImage((const uchar*)(Rgb.data), Rgb.cols, Rgb.rows, Rgb.cols * Rgb.channels(), QImage::Format_RGB888);
		q_image = QImage((const uchar*)(image.data), image.cols, image.rows, image.cols*image.channels(), QImage::Format_Indexed8);
	}
	QImage imgScaled;
	imgScaled = q_image.scaled(show_image->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//饱满缩放
	//imgScaled = img.scaled(show_image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);//保持原图的宽高比
	show_image->setPixmap(QPixmap::fromImage(imgScaled));

}
//(d)读取显示数据
void GrapImgTof::ReadData() {
	std::vector<cv::Mat> m_depth_mats(1, cv::Mat::zeros(480, 640, CV_8UC1));//depth图像
	std::vector<cv::Mat> m_ir_mats(1, cv::Mat::zeros(480, 640, CV_8UC1));//ir图像
	long pre_count = 0;//前一帧图像id
	while (m_open_flag) {
		if (m_open_flag == false) {
			break;
		}
		//是否接收到新数据
		if (m_oflim_listener->cameraListenerVec[0].tof_openFlag == false) {//相机未打开
			continue;
		}
		if (pre_count == m_oflim_listener->cameraListenerVec[0].frame_count) {
			cv::waitKey(10);
			continue;
		}
		pre_count = m_oflim_listener->cameraListenerVec[0].frame_count;
		//2-1)获得图像
		m_oflim_listener->cameraListenerVec[0].Ir_Image.copyTo(m_ir_mats[0]);
		m_oflim_listener->cameraListenerVec[0].Depth_Image.copyTo(m_depth_mats[0]);
		if (ui->comboBoxFlip->currentText() == "水平翻转") {//左右翻转
			cv::flip(m_ir_mats[0], m_ir_mats[0], 1);
			cv::flip(m_depth_mats[0], m_depth_mats[0], 1);
		}
		if (ui->comboBoxFlip->currentText() == "垂直翻转") {//上下翻转
			cv::flip(m_ir_mats[0], m_ir_mats[0], 0);
			cv::flip(m_depth_mats[0], m_depth_mats[0], 0);
		}
		F_gammaTransform(m_ir_mats[0], m_ir_mats[0], ui->textEditGamma->toPlainText().toDouble());
		//2-2)显示图像
		ShowMat(m_depth_mats[0], ui->labelDepth);//界面显示
		ShowMat(m_ir_mats[0], ui->labelIR);
		//2-3)保存图像至本地文件夹
		if (m_save_flag == true) {
			//获取当前的时间
			QDateTime current_date_time = QDateTime::currentDateTime();
			QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");

			
			if (ui->comboBoxImgs->currentText() == "jpg") {
				cv::imwrite(path.toStdString() + "/IR/" + current_date.toStdString() + ".jpg", m_ir_mats[0], jpg_quality);
				cv::imwrite(path.toStdString() + "/Depth/" + current_date.toStdString() + ".jpg", m_depth_mats[0], jpg_quality);
			}
			else {
				cv::imwrite(path.toStdString() + "/Depth/" + current_date.toStdString() + ".bmp", m_ir_mats[0]);
				cv::imwrite(path.toStdString() + "/IR/" + current_date.toStdString() + ".bmp", m_depth_mats[0]);
			}
		}
		cv::waitKey(10);
	}

}
//(f)关闭窗口事件
void GrapImgTof::closeEvent(QCloseEvent *event) {
	m_save_flag = false;
	Sleep(100);
	m_open_flag = false;
	Sleep(100);
	for (int i = 0; i < m_tof_num; i++) {
		if (m_oflim_listener->cameraListenerVec[i].tof_openFlag) {//打开成功
			m_oflim_listener->tofEnd(i);
		}
	}
}


#pragma region 槽函数
//1、寻找相机设备
void GrapImgTof::SearchClicked() {
	//搜索按钮不可点
	ui->BtnSearch->setEnabled(false);
	ui->BtnOpen->setEnabled(false);
	ui->textEditPrint->clear();
	ui->textEditPrint->append("Search Tof ...");
	//重新定义类变量
	m_oflim_listener->~OflimListener();
	m_oflim_listener = new OflimListener();
	m_oflim_listener->tofSearch();//搜索相机
	m_tof_num = m_oflim_listener->cameraListenerVec.size();
	ui->textEditPrint->append("Detect " + QString::number(m_tof_num) + " camera(s).");
	if (m_tof_num == 0) {
		ui->BtnOpen->setEnabled(false);
		qDebug() << "FALSE";
	}
	else {
		for (int i = 0; i < m_tof_num; i++) {
			m_oflim_listener->tofInit(i);
			std:string temp_str = m_oflim_listener->cameraListenerVec[i].tof_cameraID.toStdString();
			ui->textEditPrint->append(F_str2qstr(temp_str));
		}
		ui->BtnOpen->setEnabled(true);
	}
	//按钮可点击
	//ui->BtnSearch->setText("SearchTof");
	ui->BtnSearch->setEnabled(true);
}
//2、打开第一个相机
void GrapImgTof::OpenFirstCam() {
	if (ui->BtnOpen->text() == "OpenTof") {
		if (m_oflim_listener->cameraListenerVec[0].tof_initFlag == true) {
			if (m_oflim_listener->cameraListenerVec[0].tof_openFlag == false) {//初始化成功、未打开相机
				m_oflim_listener->tofStart(0);//打开相机
			}
		}
		//设置距离参数
		m_oflim_listener->cameraListenerVec[0].near_dis = ui->textEditMinD->toPlainText().toInt();
		m_oflim_listener->cameraListenerVec[0].far_dis = ui->textEditMaxD->toPlainText().toInt();
		m_open_flag = true;
		ui->BtnOpen->setText("CloseTof");
		ui->BtnSave->setEnabled(true);
		ui->BtnSearch->setEnabled(false);
		ReadData();
	}
	else {
		//停止保存
		m_save_flag = false;
		ui->BtnSave->setText("StartSave");
		ui->BtnSave->setEnabled(false);
		//关闭显示相机
		timer_->stop();
		time_.setHMS(0, 0, 0, 0);
		m_open_flag = false;
		ui->BtnOpen->setText("OpenTof");
		ui->BtnSearch->setEnabled(true);
	}
}
//3、开始保存
void GrapImgTof::StartSaveClicked() {
	if (ui->BtnSave->text() == "StartSave") {
		timer_->start(1000);
		if (m_open_flag == false) {
			QMessageBox::about(NULL, "警告", "请先打开相机！");//弹窗
			return;
		}
		//保存文件名
		if (ui->textEditDir->toPlainText()=="") {
			QMessageBox::about(NULL, "警告", "请输入保存文件夹名！");//弹窗
			return;
		}
		else
		{	
			//参数保存格式转换、数值获取
			QString temp_id = QString::fromStdString(m_oflim_listener->cameraListenerVec[0].tof_cameraID.toStdString());//相机id
			if (ui->comboBoxFlip->currentText() == "水平翻转") {
				horizontal_flip = true;
			}
			else if (ui->comboBoxFlip->currentText() == "垂直翻转") {
				vertical_flip = true;
			}
			else
			{
				horizontal_flip = false;
				vertical_flip = false;
			}//翻转类型
			int disFar_ = m_oflim_listener->cameraListenerVec[0].far_dis;
			int disNear_ = m_oflim_listener->cameraListenerVec[0].near_dis;//depth距离
			//创建、保存文件夹、config文件
			QString depth_path = path + "/Depth";
			QString ir_path = path + "/IR";
			F_createFile(path+ "/label.txt");
			F_createDir(depth_path);
			F_createDir(ir_path);
			F_createFile(path + "/Camera.INI");
			F_saveQstring_INI(path+ "/Camera.INI", "Camera", "id", temp_id);
			F_saveBool_INI(path + "/Camera.INI", "Camera", "vertical", vertical_flip);
			F_saveBool_INI(path + "/Camera.INI", "Camera", "level", horizontal_flip);
			F_saveInt_INI(path + "/Camera.INI", "Camera", "neardis", disNear_);
			F_saveInt_INI(path + "/Camera.INI", "Camera", "fardis", disFar_);
			QFile file;
			file.setFileName(path +"/label.txt");
			if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream stream(&file);
				stream << ui->comboBoxHand->currentText();
				file.close();
			}
		}
		
		m_save_flag = true;
		ui->BtnSave->setText("EndSave");
	}
	else {
		time_count = 0;
		timer_->stop();
		time_.setHMS(0, 0, 0, 0);
		counts = 0;
		id = 0;
		m_save_flag = false;
		ui->BtnSave->setText("StartSave");
	}
}
//4、创建文件夹
void GrapImgTof::CreateDir() {
	//QString temp= QDateTime::currentDateTime().toString("yyyy_MM_dd_mm_ss");
	ui->textEditDir->setText(ui->comboBoxHand->currentText()+ "_"+ QDateTime::currentDateTime().toString("yyyy_MM_dd_mm_ss"));
	path= GetSavePath();
}
//5、定时器
void GrapImgTof::update()
{
	//static quint32 time_out = 0;
	time_count++;
	time_ = time_.addSecs(1);
	ui->labelBar->setText("时间："+QString::number(time_count)+"s");
	if ((time_count / 60) == ui->textEditWait->toPlainText().toInt()) {
		time_count = 0;
		timer_->stop();
		time_.setHMS(0, 0, 0, 0);
		counts = 0;
		id = 0;
		m_save_flag = false;
		ui->BtnSave->setText("StartSave");
	}
}

#pragma endregion
#pragma region 样式
void GrapImgTof::on_pushButton_pressed() {
	ui->BtnSearch->setGeometry(ui->BtnSearch->x() + 5, ui->BtnSearch->y() + 5, ui->BtnSearch->width() - 10, ui->BtnSearch->height() - 10);
}
void GrapImgTof::on_pushButton_released() {
	ui->BtnSearch->setGeometry(ui->BtnSearch->x() - 5, ui->BtnSearch->y() - 5, ui->BtnSearch->width() + 10, ui->BtnSearch->height() + 10);
}
void GrapImgTof::on_pushButton1_pressed() {
	ui->BtnOpen->setGeometry(ui->BtnOpen->x() + 5, ui->BtnOpen->y() + 5, ui->BtnOpen->width() - 10, ui->BtnOpen->height() - 10);
}
void GrapImgTof::on_pushButton1_released() {
	ui->BtnOpen->setGeometry(ui->BtnOpen->x() - 5, ui->BtnOpen->y() - 5, ui->BtnOpen->width() + 10, ui->BtnOpen->height() + 10);
}

#pragma endregion
