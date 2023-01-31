#include "SelectData.h"
#include "QtCommonFunction.cpp"//QT�ļ�



SelectData::SelectData(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::SelectDataClass)
{
	ui->setupUi(this);
	//�ۺ���
	connect(ui->brushBtn, SIGNAL(clicked()), this, SLOT(brush_clicked()));
	//˫�򻬶�����
	rangeSliderThre = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, nullptr);
	QHBoxLayout* rangeSliderWidgetSpaceLayout = new QHBoxLayout();
	rangeSliderWidgetSpaceLayout->addWidget(rangeSliderThre);
	ui->rangeSliderWidgetSpace->setLayout(rangeSliderWidgetSpaceLayout);
	ui->slidmin->setText(QString::number(rangeSliderThre->GetMinimun()));
	InitParam();
}
#pragma region �ؼ�����
//ѡ�����ݼ�
void SelectData::btn_ChooseData() {
	//1)ѡ���ļ���
	temp_qstr = QFileDialog::getExistingDirectory(NULL, "ѡ��ĳ�����Ƶ����ݼ�", m_exe_path);
	if (temp_qstr.isEmpty()) {
		return;
	}
	ui->TXpath->setText(temp_qstr);
	label_info = readTXT(temp_qstr + "//label.txt");
	m_depth_imgs = F_loadFileImages(temp_qstr + "//Depth");
	m_ir_imgs = F_loadFileImages(temp_qstr + "//IR");
	ui->TXefficient->append(label_info);
	if (m_depth_imgs.size() != m_ir_imgs.size())
		return;
	else
	{
		//���õ���������
		rangeSliderThre->SetMaximum(m_depth_imgs.size());
		ui->slidmax->setText(QString::number(rangeSliderThre->GetMaximun()));
	}
}
//��ȡͼ��
void SelectData::ReadImg() {
	if (ui->StartBtn->text() == tr("����")) {
		ui->StartBtn->setText(tr("��ͣ"));
		ui->lowvalue->setText(QString::number(rangeSliderThre->GetLowerValue()));
		ui->upvalue->setText(QString::number(rangeSliderThre->GetUpperValue()));
		if (m_ir_imgs.size() == 0 && m_depth_imgs.size() == 0)
			return;
		//���Ż�����ͼ��
		for (int i = rangeSliderThre->GetLowerValue(); i < rangeSliderThre->GetUpperValue(); i++) {
			Mat img_ir = imread(m_ir_imgs[i]);
			Mat img_depth = imread(m_depth_imgs[i]);
			F_showMat(img_ir, ui->label);
			s_ir_imgs.push_back(img_ir);
			s_depth_imgs.push_back(img_depth);
			waitKey(30);
		}
		ui->StartBtn->setText(tr("����"));
	}
	else
	{
		s_ir_imgs.clear();
		s_depth_imgs.clear();
		ui->StartBtn->setText(tr("����"));
		waitKey(0);
	}

	
}
//ѡ���Ƿ񱣴�
void SelectData::endclicked() {
	switch (QMessageBox::information(this, "Warning", tr("Save imgs to local?"),
		QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save))
	{
	case QMessageBox::Save:
		saveData();
		break;
	case QMessageBox::Cancel:
		s_depth_imgs.clear();
		s_ir_imgs.clear();
		break;
	default:
		break;
	}
}
//����vector<Mat>����
void SelectData::brush_clicked() {
	s_ir_imgs.clear();
	s_depth_imgs.clear();
}

void SelectData::lowup_clicked() {
	int tmpValue = rangeSliderThre->GetLowerValue() + sliderLen;
	rangeSliderThre->SetLowerValue(tmpValue);
}
void SelectData::lowdown_clicked() {
	int tmpValue = rangeSliderThre->GetLowerValue() - sliderLen;
	rangeSliderThre->SetLowerValue(tmpValue);
}
void SelectData::upup_clicked() {
	int tmpValue = rangeSliderThre->GetUpperValue() + sliderLen;
	rangeSliderThre->SetUpperValue(tmpValue);
}
void SelectData::updown_clicked(){
	int tmpValue = rangeSliderThre->GetUpperValue() - sliderLen;
	rangeSliderThre->SetUpperValue(tmpValue);
}
#pragma endregion

//������Ч����
void SelectData::saveData() {
	if (s_depth_imgs.size() > 0 && s_ir_imgs.size() > 0) {
		QDateTime current_date_time = QDateTime::currentDateTime();
		QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");
		QString saveDir = m_exe_path + "/SaveData";
		QString imgDir = saveDir + "/" + label_info + "_" + current_date;
		F_createDir(saveDir);
		F_createDir(imgDir);
		F_createDir(imgDir+"/IR");
		F_createDir(imgDir + "/Depth");
		QFile::copy(temp_qstr + "//label.txt", saveDir + "/" + label_info + "_" + current_date + ".txt");
		QFile::copy(temp_qstr + "//Camera.INI", imgDir + "/Camera.INI");
		for (int i = 0; i < s_depth_imgs.size(); i++) {
			QString tmp_time = QDateTime::currentDateTime() .toString("yyyy_MM_dd_hh_mm_ss_zzz");
			String tmp_irname = (imgDir + "/IR/"+label_info + "_" + tmp_time+".jpg").toStdString();
			String tmp_depthname = (imgDir + "/Depth/" + label_info + "_" + tmp_time + ".jpg").toStdString();
			imwrite(tmp_irname, s_ir_imgs[i]);
			imwrite(tmp_depthname, s_depth_imgs[i]);
		}
		s_depth_imgs.clear();
		s_ir_imgs.clear();
	}
	else
	{
		return;
	}
}
//��ȡlabel txt����
QString SelectData::readTXT(QString txtPath) {
	QFile f(txtPath);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))//��ָ���ļ�
	{
		QMessageBox::about(NULL, "�ļ�", "�ļ���ʧ��");
	}

	QTextStream txtInput(&f);
	QString lineStr;
	while (!txtInput.atEnd())
	{
		lineStr = txtInput.readLine();  //��ȡ����
		QMessageBox::about(NULL, "�ֶ�����", lineStr);
	}

	f.close();
	return lineStr;
}
//��ʼ������
void SelectData::InitParam() {
	//exe����·��
	m_exe_path = F_getCurRunPath();
	//config·��
	m_config_path = m_exe_path + "/Config.INI";
	//���ݼ�
	m_depth_name = "Depth";
	m_ir_name = "IR";

	//��ȡConfig�ļ�
	if (!F_isFileExist(m_config_path)) {//Config�ļ�������
		F_saveQstring_INI(m_config_path, "DataBase", "depth_name", m_depth_name);
		F_saveQstring_INI(m_config_path, "DataBase", "ir_name", m_ir_name);

	}
	else {
		F_loadQstring_INI(m_config_path, "DataBase", "depth_name", m_depth_name);
		F_loadQstring_INI(m_config_path, "DataBase", "ir_name", m_ir_name);
		F_loadInt_INI(m_config_path, "UiParam","sliderL", sliderLen);
	}

	m_depth_imgs.clear();
	m_ir_imgs.clear();
	s_depth_imgs.clear();
	s_ir_imgs.clear();
}
//�رմ���
void SelectData::closeEvent(QCloseEvent *event) {
	QMessageBox::StandardButton button;
	button = QMessageBox::question(this, tr("�˳�����"),
		QString(tr("���棺������һ���������������У��Ƿ���������˳�?")),
		QMessageBox::Yes | QMessageBox::No);

	if (button == QMessageBox::No) {
		event->ignore();  //�����˳��źţ������������
	}
	else if (button == QMessageBox::Yes) {
		event->accept();  //�����˳��źţ������˳�
	}
	
}
