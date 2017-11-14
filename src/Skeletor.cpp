#include "MainWindow.h"

#include "QApplication"
#include "QPushButton"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	MainWindow *window = new MainWindow();

	window->resize(800, 600);
	window->showFullScreen();
	window->showMaximized();
	window->show();

	window->reset();

    return app.exec();
}