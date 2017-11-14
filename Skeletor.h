#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Skeletor.h"

class Skeletor : public QMainWindow
{
	Q_OBJECT

public:
	Skeletor(QWidget *parent = Q_NULLPTR);

private:
	Ui::SkeletorClass ui;
};
