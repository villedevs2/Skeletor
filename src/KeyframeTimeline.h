#pragma once

#include <QtGui>
#include <qwidget.h>
#include <vector>

using namespace std;

class KeyframeTimeline : public QWidget
{
	Q_OBJECT

public:
	KeyframeTimeline(QWidget *parent);
	~KeyframeTimeline();

	void setRange(int start, int end);
	void reset();
	void insert(int time);
	int getNumFrames();
	int getFrame(int index);

protected:
	void paintEvent(QPaintEvent *event);
	vector<int> m_frames;

	int m_start;
	int m_end;
};