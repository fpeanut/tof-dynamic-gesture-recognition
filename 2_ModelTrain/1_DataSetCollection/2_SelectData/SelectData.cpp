#include "SelectData.h"
#include "QtCommonFunction.cpp"//QT文件



SelectData::SelectData(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::SelectDataClass)
{
	ui->setupUi(this);
	//槽函数
	connect(ui->brushBtn, SIGNAL(clicked()), this, SLOT(brush_clicked()));
	//双向滑动窗口
	rangeSliderThre = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, nullptr);
	QHBoxLayout* rangeSliderWidgetSpaceLayout = new QHBoxLayout();
	rangeSliderWidgetSpaceLayout->addWidget(rangeSliderThre);
	ui->rangeSliderWidgetSpace->setLayout(rangeSliderWidgetSpaceLayout);
	ui->slidmin->setText(QString::number(rangeSliderThre->GetMinimun()));
	InitParam();
}
#pragma region 控件函数
//选择数据集
void SelectData::btn_ChooseData() {
	//1)选择文件夹
	temp_qstr = QFileDialog::getExistingDirectory(NULL, "选择某个手势的数据集", m_exe_path);
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
		//设置导航条长度
		rangeSliderThre->SetMaximum(m_depth_imgs.size());
		ui->slidmax->setText(QString::number(rangeSliderThre->GetMaximun()));
	}
}
//读取图像
void SelectData::ReadImg() {
	if (ui->StartBtn->text() == tr("播放")) {
		ui->StartBtn->setText(tr("暂停"));
		ui->lowvalue->setText(QString::number(rangeSliderThre->GetLowerValue()));
		ui->upvalue->setText(QString::number(rangeSliderThre->GetUpperValue()));
		if (m_ir_imgs.size() == 0 && m_depth_imgs.size() == 0)
			return;
		//播放滑块内图像
		for (int i = rangeSliderThre->GetLowerValue(); i < rangeSliderThre->GetUpperValue(); i++) {
			Mat img_ir = imread(m_ir_imgs[i]);
			Mat img_depth = imread(m_depth_imgs[i]);
			F_showMat(img_ir, ui->label);
			s_ir_imgs.push_back(img_ir);
			s_depth_imgs.push_back(img_depth);
			waitKey(30);
		}
		ui->StartBtn->setText(tr("播放"));
	}
	else
	{
		s_ir_imgs.clear();
		s_depth_imgs.clear();
		ui->StartBtn->setText(tr("播放"));
		waitKey(0);
	}

	
}
//选择是否保存
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
//擦除vector<Mat>数据
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

//保存有效数据
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
//读取label txt内容
QString SelectData::readTXT(QString txtPath) {
	QFile f(txtPath);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))//打开指定文件
	{
		QMessageBox::about(NULL, "文件", "文件打开失败");
	}

	QTextStream txtInput(&f);
	QString lineStr;
	while (!txtInput.atEnd())
	{
		lineStr = txtInput.readLine();  //读取数据
		QMessageBox::about(NULL, "字段数据", lineStr);
	}

	f.close();
	return lineStr;
}
//初始化参数
void SelectData::InitParam() {
	//exe运行路径
	m_exe_path = F_getCurRunPath();
	//config路径
	m_config_path = m_exe_path + "/Config.INI";
	//数据集
	m_depth_name = "Depth";
	m_ir_name = "IR";

	//读取Config文件
	if (!F_isFileExist(m_config_path)) {//Config文件不存在
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
//关闭窗口
void SelectData::closeEvent(QCloseEvent *event) {
	QMessageBox::StandardButton button;
	button = QMessageBox::question(this, tr("退出程序"),
		QString(tr("警告：程序有一个任务正在运行中，是否结束操作退出?")),
		QMessageBox::Yes | QMessageBox::No);

	if (button == QMessageBox::No) {
		event->ignore();  //忽略退出信号，程序继续运行
	}
	else if (button == QMessageBox::Yes) {
		event->accept();  //接受退出信号，程序退出
	}
	
}
