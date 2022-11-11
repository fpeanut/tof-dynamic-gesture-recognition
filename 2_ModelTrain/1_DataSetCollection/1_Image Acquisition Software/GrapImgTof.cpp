#include "GrapImgTof.h"
#include "QtCommonFunction.cpp"//QT�ļ�
#include "OflimListener.cpp"//Oflim���Tof��

GrapImgTof::GrapImgTof(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::GrapImgTofClass)
{
	ui->setupUi(this);
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);//�̶����ڴ�С���޷��ı�
	//��ʱ��
	time_.setHMS(0, 0, 0, 0);//���ó�ʼֵ   
	timer_ = new QTimer(this);//����һ����ʱ��   
	connect(timer_, SIGNAL(timeout()), this, SLOT(update()));

	
	//a)���������ʼ��
	initUiParam();
	//b)��ȡͼ�񱣴��ַ
	GetSavePath();

	//)ͼ�񱣴�����
	jpg_quality.push_back(cv::IMWRITE_JPEG_QUALITY);  //ѡ��jpeg
	jpg_quality.push_back(100); //�����������Ҫ��ͼƬ����
}

//(a)���������ʼ��
void GrapImgTof::initUiParam() {
	//exe����·��
	m_exe_path = F_getCurRunPath();
	//config·��
	m_config_path = m_exe_path + "/config.ini";
	//��ȡconfig�ļ�
	F_loadQstring_INI(m_config_path, "HandGesture","handGT", m_hand_items);
	QStringList m_list = m_hand_items.split("/");
	qDebug() << m_list[0];
	//combox�˵�����
	ui->comboBoxFlip->addItem("-");
	ui->comboBoxFlip->addItem("ˮƽ��ת");
	ui->comboBoxFlip->addItem("��ֱ��ת");
	ui->comboBoxHand->addItem("-");
	for (int i = 0; i < m_list.size(); i++) {
		ui->comboBoxHand->addItem(m_list[i]);
	}
	ui->comboBoxImgs->addItem("jpg");
	ui->comboBoxImgs->addItem("bmp");
	
	//���
	for (int i = 0; i < m_tof_num; i++) {
		QString temp = QString::fromStdString(m_oflim_listener->cameraListenerVec[i].tof_cameraID.toStdString());
		ui->textEditPrint->append("���������"+temp);
	}
}
//(b)��ȡͼ�񱣴��ַ
QString GrapImgTof::GetSavePath() {
	QString currentPath = QDir::currentPath();//��ȡ��ǰ·��
	dataTime = QDateTime::currentDateTime();//��ȡ��ǰʱ��
	QString timeToday;
	timeToday = dataTime.toString("yyyy_MM_dd");
	QDir dir, dir_next0;
	QString firstPath = currentPath + "/SaveImage/";
	QString secondtPath;
	//����SaveImage
	if (!dir.exists(firstPath)) {
		dir.mkdir(firstPath);
	}
	else
	{
		qDebug() << "SaveImage�ļ����Ѵ���";
	}
	//����SaveImage/������
	if (dir.exists(firstPath)) {
		secondtPath = firstPath + timeToday + "/";
		if (!dir_next0.exists(secondtPath)) {
			dir_next0.mkdir(secondtPath);
		}
		else
		{
			qDebug() << "SaveImage/" + timeToday + "�ļ����Ѵ���";
		}
	}
	//����SaveImage/������/����
	if (dir_next0.exists(secondtPath)) {
		QDir dir_next1;
		QString thirthPath = secondtPath + ui->textEditDir->toPlainText() + "/";
		if (!dir_next1.exists(thirthPath)) {
			dir_next1.mkdir(thirthPath);
			return thirthPath;
		}
		else
		{
			qDebug() << "SaveImage/" + timeToday + "/" + ui->textEditPrint->toPlainText() + "�ļ����Ѵ���";
			return thirthPath;
		}
	}
}
//(c)��ʾMatͼ��
void GrapImgTof::ShowMat(cv::Mat image, QLabel *show_image)
{
	//ͼƬ����ʾ��Ҫ���⻹������QImage��Mat��ʽ�Ĳ�ͬ������QImage��RGB���У���Mat ��BGR��ͬʱ���߸�ʽ����������ҳ��ͬ�������Ҫ����ת�����ܹ���ʾ��
	cv::Mat rgb_image;
	QImage q_image;
	if (image.channels() == 3)//RGB Img
	{
		cv::cvtColor(image, image, /*CV_BGR2RGB*/4);//��ɫ�ռ�ת��
		q_image = QImage((const uchar*)(image.data), image.cols, image.rows, image.cols * image.channels(), QImage::Format_RGB888);
	}
	else//Gray Img
	{
		//cv::cvtColor(temp, Rgb,CV_GRAY2RGB);//��ɫ�ռ�ת��
		//Img = QImage((const uchar*)(Rgb.data), Rgb.cols, Rgb.rows, Rgb.cols * Rgb.channels(), QImage::Format_RGB888);
		q_image = QImage((const uchar*)(image.data), image.cols, image.rows, image.cols*image.channels(), QImage::Format_Indexed8);
	}
	QImage imgScaled;
	imgScaled = q_image.scaled(show_image->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//��������
	//imgScaled = img.scaled(show_image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);//����ԭͼ�Ŀ�߱�
	show_image->setPixmap(QPixmap::fromImage(imgScaled));

}
//(d)��ȡ��ʾ����
void GrapImgTof::ReadData() {
	std::vector<cv::Mat> m_depth_mats(1, cv::Mat::zeros(480, 640, CV_8UC1));//depthͼ��
	std::vector<cv::Mat> m_ir_mats(1, cv::Mat::zeros(480, 640, CV_8UC1));//irͼ��
	long pre_count = 0;//ǰһ֡ͼ��id
	while (m_open_flag) {
		if (m_open_flag == false) {
			break;
		}
		//�Ƿ���յ�������
		if (m_oflim_listener->cameraListenerVec[0].tof_openFlag == false) {//���δ��
			continue;
		}
		if (pre_count == m_oflim_listener->cameraListenerVec[0].frame_count) {
			cv::waitKey(10);
			continue;
		}
		pre_count = m_oflim_listener->cameraListenerVec[0].frame_count;
		//2-1)���ͼ��
		m_oflim_listener->cameraListenerVec[0].Ir_Image.copyTo(m_ir_mats[0]);
		m_oflim_listener->cameraListenerVec[0].Depth_Image.copyTo(m_depth_mats[0]);
		if (ui->comboBoxFlip->currentText() == "ˮƽ��ת") {//���ҷ�ת
			cv::flip(m_ir_mats[0], m_ir_mats[0], 1);
			cv::flip(m_depth_mats[0], m_depth_mats[0], 1);
		}
		if (ui->comboBoxFlip->currentText() == "��ֱ��ת") {//���·�ת
			cv::flip(m_ir_mats[0], m_ir_mats[0], 0);
			cv::flip(m_depth_mats[0], m_depth_mats[0], 0);
		}
		F_gammaTransform(m_ir_mats[0], m_ir_mats[0], ui->textEditGamma->toPlainText().toDouble());
		//2-2)��ʾͼ��
		ShowMat(m_depth_mats[0], ui->labelDepth);//������ʾ
		ShowMat(m_ir_mats[0], ui->labelIR);
		//2-3)����ͼ���������ļ���
		if (m_save_flag == true) {
			//��ȡ��ǰ��ʱ��
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
//(f)�رմ����¼�
void GrapImgTof::closeEvent(QCloseEvent *event) {
	m_save_flag = false;
	Sleep(100);
	m_open_flag = false;
	Sleep(100);
	for (int i = 0; i < m_tof_num; i++) {
		if (m_oflim_listener->cameraListenerVec[i].tof_openFlag) {//�򿪳ɹ�
			m_oflim_listener->tofEnd(i);
		}
	}
}


#pragma region �ۺ���
//1��Ѱ������豸
void GrapImgTof::SearchClicked() {
	//������ť���ɵ�
	ui->BtnSearch->setEnabled(false);
	ui->BtnOpen->setEnabled(false);
	ui->textEditPrint->clear();
	ui->textEditPrint->append("Search Tof ...");
	//���¶��������
	m_oflim_listener->~OflimListener();
	m_oflim_listener = new OflimListener();
	m_oflim_listener->tofSearch();//�������
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
	//��ť�ɵ��
	//ui->BtnSearch->setText("SearchTof");
	ui->BtnSearch->setEnabled(true);
}
//2���򿪵�һ�����
void GrapImgTof::OpenFirstCam() {
	if (ui->BtnOpen->text() == "OpenTof") {
		if (m_oflim_listener->cameraListenerVec[0].tof_initFlag == true) {
			if (m_oflim_listener->cameraListenerVec[0].tof_openFlag == false) {//��ʼ���ɹ���δ�����
				m_oflim_listener->tofStart(0);//�����
			}
		}
		//���þ������
		m_oflim_listener->cameraListenerVec[0].near_dis = ui->textEditMinD->toPlainText().toInt();
		m_oflim_listener->cameraListenerVec[0].far_dis = ui->textEditMaxD->toPlainText().toInt();
		m_open_flag = true;
		ui->BtnOpen->setText("CloseTof");
		ui->BtnSave->setEnabled(true);
		ui->BtnSearch->setEnabled(false);
		ReadData();
	}
	else {
		//ֹͣ����
		m_save_flag = false;
		ui->BtnSave->setText("StartSave");
		ui->BtnSave->setEnabled(false);
		//�ر���ʾ���
		timer_->stop();
		time_.setHMS(0, 0, 0, 0);
		m_open_flag = false;
		ui->BtnOpen->setText("OpenTof");
		ui->BtnSearch->setEnabled(true);
	}
}
//3����ʼ����
void GrapImgTof::StartSaveClicked() {
	if (ui->BtnSave->text() == "StartSave") {
		timer_->start(1000);
		if (m_open_flag == false) {
			QMessageBox::about(NULL, "����", "���ȴ������");//����
			return;
		}
		//�����ļ���
		if (ui->textEditDir->toPlainText()=="") {
			QMessageBox::about(NULL, "����", "�����뱣���ļ�������");//����
			return;
		}
		else
		{	
			//���������ʽת������ֵ��ȡ
			QString temp_id = QString::fromStdString(m_oflim_listener->cameraListenerVec[0].tof_cameraID.toStdString());//���id
			if (ui->comboBoxFlip->currentText() == "ˮƽ��ת") {
				horizontal_flip = true;
			}
			else if (ui->comboBoxFlip->currentText() == "��ֱ��ת") {
				vertical_flip = true;
			}
			else
			{
				horizontal_flip = false;
				vertical_flip = false;
			}//��ת����
			int disFar_ = m_oflim_listener->cameraListenerVec[0].far_dis;
			int disNear_ = m_oflim_listener->cameraListenerVec[0].near_dis;//depth����
			//�����������ļ��С�config�ļ�
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
//4�������ļ���
void GrapImgTof::CreateDir() {
	//QString temp= QDateTime::currentDateTime().toString("yyyy_MM_dd_mm_ss");
	ui->textEditDir->setText(ui->comboBoxHand->currentText()+ "_"+ QDateTime::currentDateTime().toString("yyyy_MM_dd_mm_ss"));
	path= GetSavePath();
}
//5����ʱ��
void GrapImgTof::update()
{
	//static quint32 time_out = 0;
	time_count++;
	time_ = time_.addSecs(1);
	ui->labelBar->setText("ʱ�䣺"+QString::number(time_count)+"s");
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
#pragma region ��ʽ
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
