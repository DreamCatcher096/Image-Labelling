
#include "imagelabelling.h"
#include <QtGui\QImageReader>
#include <opencv2\imgproc\imgproc.hpp>
#include <QtWidgets>
using namespace cv;

ImageLabelling::ImageLabelling(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	timer = new QTimer(this);
	ui.label->setMouseTracking(true);
	ui.label->setAlignment(Qt::AlignCenter);
	connect(ui.start, SIGNAL(clicked()), this, SLOT(openFilename()));
	connect(timer, SIGNAL(timeout()), this, SLOT(RefreshImage()));
	connect(ui.Button1, SIGNAL(clicked()), this, SLOT(GetNextImage()));
	connect(ui.last, SIGNAL(clicked()), this, SLOT(GetLastImage()));
	connect(ui.Buttons, &QAbstractButton::clicked, this, &QWidget::close);
}

ImageLabelling::~ImageLabelling()
{

}

void ImageLabelling::openFilename()
{
	//´ò¿ª¶Ô»°¿ò£¬»ñÈ¡ËùÑ¡ÔñµÄÎÄ¼þ¼Ð
	ImagePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "C:", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	string tempstring = ImagePath.toStdString();
	//ÅÐ¶ÏÊÇ·ñÑ¡ÔñÁËÎÄ¼þ¼Ð  
	if ("" == ImagePath) return;
	//ImagePath = fileName.toStdString();
	InitEnvir();
}

void ImageLabelling::GetNextImage()
{
	int rx, ry;
	for (int i = 0; i < Pcount; i++) {
		allpoints.push_back(points[i]);
	}
	points_amount.push_back(Pcount);
	outFile.open(filesname, ios::out | ios::binary);
	if (!outFile.is_open()) {
		cout << "Can't open res.txt" << endl;
		exit(0);
	}
	int counter = 0, index = 0;
	for (int i = 0; i < allpoints.size(); i++) {
		counter++;
		if (counter > points_amount[index]) {
			counter = 1;
			index++;
		}
		rx = allpoints[i].x;
		ry = allpoints[i].y;
		string tempnamee = filenames->at(index).filePath().toStdString();
		vector<string> temp;
		Point temp_point;
		string key = "/";
		string_split(tempnamee, key, &temp);
		outFile << temp[temp.size() - 1] + " ";
		outFile << rx;
		outFile << " ";
		outFile << ry;
		outFile << " \n";
	}
	outFile.close();
	for (int i = 0; i < MAX_BOX; i++)
		points[i] = Point(-1, -1);

	ofstream numberRecord;
	numberRecord.open(ImagePath.toStdString() + "/" + "num.txt");
	numberRecord << Icount << endl;
	for (int i = 0; i < points_amount.size(); i++) {
		int temp = points_amount[i];
		numberRecord << temp << endl;
	}
	numberRecord.close();
	Pcount = 0;
	if (Icount < fileNumber)
		imageName = filenames->at(Icount++).filePath().toStdString();
	else {
		QMessageBox::information(this, tr("Completed!"),
			tr("The labelling process is comlpeted!"));
		close();
		return;
	}

	
	srcImage = imread(imageName);
	double rate;
	if (srcImage.rows > srcImage.cols)
		rate = srcImage.rows;
	else
		rate = srcImage.cols;
	if (rate > 1000) {
		rate = 1000 / rate;
	}
	cv::resize(srcImage, tempImage, Size(), rate, rate, 3);
	tempImage.copyTo(srcImage);
	image_height = srcImage.rows;
	image_width = srcImage.cols;
	ShowImage(srcImage);

	QString str = "Progress: " + QString::number(Icount) + "/" + QString::number(fileNumber);
	ui.label_2->setText(str);
}

void ImageLabelling::GetLastImage()
{
	if (Icount > 0) {
		Icount -= 2;
		imageName = filenames->at(Icount++).filePath().toStdString();
		int last_point_amount = points_amount[points_amount.size() - 1];
		points_amount.pop_back();
		for (int i = 0; i < last_point_amount; i++)
			allpoints.pop_back();
		for (int i = 0; i < MAX_BOX; i++)
			points[i] = Point(-1, -1);
		srcImage = imread(imageName);
		double rate;
		if (srcImage.rows > srcImage.cols)
			rate = srcImage.rows;
		else
			rate = srcImage.cols;
		if (rate > 1000) {
			rate = 1000 / rate;
		}
		cv::resize(srcImage, tempImage, Size(), rate, rate, 3);
		tempImage.copyTo(srcImage);
		image_height = srcImage.rows;
		image_width = srcImage.cols;
		ShowImage(srcImage);
		QString str = "Progress: " + QString::number(Icount) + "/" + QString::number(fileNumber);
		ui.label_2->setText(str);
	}
	else {
		return;
	}
}

void ImageLabelling::InitEnvir()
{
	Icount = 0;
	timer->start(33);
	allpoints.clear();
	points_amount.clear();
	dir = new QDir(ImagePath);
	filter << "*.jpg";
	filesname = (ImagePath + "/res.txt").toStdString();
	QFile prev_res(ImagePath + "/res.txt");
	if (prev_res.exists()) {
		ifstream previos_res;
		previos_res.open(filesname);
		ifstream read_res;
		read_res.open(filesname);
		string previous_label_result;
		while (getline(read_res, previous_label_result)) {
			vector<string> temp;
			Point temp_point;
			string key = " ";
			string_split(previous_label_result, key, &temp);
			int tx, ty;
			tx = atoi(temp[1].c_str());
			ty = atoi(temp[2].c_str());
			temp_point = Point(tx, ty);
			allpoints.push_back(temp_point);
		}
		previos_res.close();
	}
	filenames = new QList<QFileInfo>(dir->entryInfoList(filter));
	fileNumber = filenames->count();
	for (int i = 0; i < MAX_BOX; i++)
		points[i] = Point(-1, -1);
	g_point = Point(-1, -1);
	QString progress_file = ImagePath + "/num.txt";
	QFile progress_file_handle(progress_file);

	if (!progress_file_handle.exists()) {
		fopen.open(progress_file.toStdString());
		imageName = filenames->at(Icount++).filePath().toStdString();
	}
	else {
		fopen.open(progress_file.toStdString());
		string LabeledNum;
		getline(fopen, LabeledNum);
		Icount = atoi(LabeledNum.c_str());
		if (Icount == fileNumber) {
			QMessageBox::information(this, tr("Completed!"),
				tr("The labelling process is comlpeted!"));
			close();
			return;
		}
		imageName = filenames->at(Icount++).filePath().toStdString();
		while (getline(fopen, LabeledNum)) {
			int temp = atoi(LabeledNum.c_str());
			points_amount.push_back(temp);
		}
	}
	QString str = "Progress: " + QString::number(Icount) + "/" + QString::number(fileNumber);
	ui.label_2->setText(str);
	srcImage = imread(imageName);
	double rate;
	if (srcImage.rows > srcImage.cols)
		rate = srcImage.rows;
	else
		rate = srcImage.cols;
	if (rate > 1000) {
		rate = 1000 / rate;
	}
	cv::resize(srcImage, tempImage, Size(), rate, rate, 3);
	tempImage.copyTo(srcImage);
	image_height = srcImage.rows;
	image_width = srcImage.cols;
	ShowImage(srcImage);
}

void ImageLabelling::DrawPoint(cv::Mat& img, cv::Point point)
{
	cv::circle(img, point, 3, cv::Scalar(0, 255, 0), CV_FILLED);
}


void ImageLabelling::RefreshImage()
{
	QPoint location = cursor().pos();
	location = ui.label->mapFromGlobal(location);
	mx = location.x() - xoffset;
	my = location.y() - yoffset;
	Point x1 = Point(mx, 0);
	Point y1 = Point(mx, image_height);
	Point x2 = Point(0, my);
	Point y2 = Point(image_width, my);
	srcImage.copyTo(tempImage);//¿½±´Ô´Í¼µ½ÁÙÊ±±äÁ¿
	ShowImage(srcImage);
	line(tempImage, x1, y1, cv::Scalar(0, 255, 0));
	line(tempImage, x2, y2, cv::Scalar(0, 255, 0));
	for (int i = 0; i < Pcount; i++)
		DrawPoint(tempImage, points[i]);
	ShowImage(tempImage);
	int t1, t2;
	t1 = ui.label->contentsRect().width();
	t2 = ui.label->pixmap()->rect().width();
	xoffset = (t1 - t2) / 2;
	t1 = ui.label->contentsRect().height();
	t2 = ui.label->pixmap()->rect().height();
	yoffset = (t1 - t2) / 2;
}

void ImageLabelling::ShowImage(Mat image)
{

	QImage qimage = cvMat2QImage(image);
	ui.label->setPixmap(QPixmap::fromImage(qimage));
	//ui.label->resize(qimage.width(), qimage.height());
}


void ImageLabelling::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QPoint location = event->globalPos();
		location = ui.label->mapFromGlobal(location);
		int tempx = location.x() - xoffset;
		int tempy = location.y() - yoffset;
		if (tempx >= 0 && tempx < image_height && tempy >= 0 && tempy < image_width) {
			g_point = Point(tempx, tempy);
			DrawPoint(image, g_point);
			points[Pcount++] = g_point;
		}
	}
}

Mat ImageLabelling::QImage2cvMat(QImage image)
{
	cv::Mat mat;
	qDebug() << image.format();
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}

QImage ImageLabelling::cvMat2QImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1  
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)  
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat  
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3  
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		qDebug() << "CV_8UC4";
		// Copy input Mat  
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat  
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		qDebug() << "ERROR: Mat could not be converted to QImage.";
		return QImage();
	}
}

void ImageLabelling::string_split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}
