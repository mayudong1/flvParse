#include "flvparse.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	flvParse w;
	w.show();
	return a.exec();
}
