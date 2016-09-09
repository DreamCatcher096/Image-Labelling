#ifndef IMAGELABELLING_H
#define IMAGELABELLING_H

#include <QtWidgets\QMainWindow>
#include <QtCore\QDebug>
#include "ui_imagelabelling.h"
#include <QtWidgets\QFileDialog>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <QtCore\QString>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <opencv2\opencv.hpp>
#include <QMouseEvent>
#include <QDir>
#include <QTimer>

using namespace std;
using namespace cv;
#define MAX_BOX 100

class ImageLabelling : public QMainWindow
{
	Q_OBJECT

public:
	ImageLabelling(QWidget *parent = 0);
	~ImageLabelling();

private slots:
    void openFilename();
	Mat QImage2cvMat(QImage image);
	QImage cvMat2QImage(const cv::Mat& mat);
	void InitEnvir();
	void GetNextImage();
	void ShowImage(Mat image);
	void RefreshImage();
	void DrawPoint(cv::Mat& img, cv::Point point);
	void GetLastImage();
	void string_split(std::string& s, std::string& delim, std::vector< std::string >* ret);

protected:
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	Ui::ImageLabellingClass ui;
	Point g_point;
	Point points[MAX_BOX];
	vector<Point> allpoints;
	vector<int> points_amount;
	int Pcount = 0;
	int Icount = 1;
	double image_width, image_height, mx, my, xoffset, yoffset;
	QString ImagePath;
	//string Extension = "*.jpg";
	bool addPath = false; 
	int fileNumber;
	Mat srcImage, tempImage;
	ifstream fopen;
	ofstream outFile;
	string filesname;
	//vector<string> filenames;// = nullptr;
	string imageName;
	QDir *dir;
	QStringList filter;
	QList<QFileInfo> *filenames;
	QTimer *timer;
	Mat tempimage, image;
};

#endif // IMAGELABELLING_H
