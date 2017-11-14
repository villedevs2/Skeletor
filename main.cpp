#include "Skeletor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Skeletor w;
	w.show();
	return a.exec();
}
