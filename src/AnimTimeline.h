#pragma once

#include <QtGui>
#include <qslider.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qgridlayout.h>
#include "KeyframeTimeline.h"

class AnimTimeline : public QWidget
{
	Q_OBJECT

public:
	AnimTimeline(QWidget *parent);
	~AnimTimeline(void);

signals:
	void sliderChanged(int value);

public slots:
	void resetKeyframes();
	void insertKeyframe(int time);
	void setRange(int start, int end);
	void setFrameSliderText(int);
	void onPrevFrame();
	void onNextFrame();

private:
	QSlider *m_slider;
	KeyframeTimeline *m_timeline;
	QGroupBox *m_group;
	QLineEdit *m_frame_text;
	QPushButton *m_prev;
	QPushButton *m_next;

	int m_current_frame;
};