#include "imagelabelling.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ImageLabelling w;
	w.show();
	return a.exec();
}
