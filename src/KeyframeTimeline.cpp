#include "KeyframeTimeline.h"

KeyframeTimeline::KeyframeTimeline(QWidget *parent) : QWidget(parent)
{
	setMinimumHeight(70);

	setRange(0, 1000);
}

KeyframeTimeline::~KeyframeTimeline()
{
}

void KeyframeTimeline::setRange(int start, int end)
{
	m_start = start;
	m_end = end;

	repaint();
}

void KeyframeTimeline::reset()
{
	m_frames.clear();

	repaint();
}

void KeyframeTimeline::insert(int time)
{
	m_frames.push_back(time);

	repaint();
}

int KeyframeTimeline::getNumFrames()
{
	return m_frames.size();
}

int KeyframeTimeline::getFrame(int index)
{
	return m_frames[index];
}

void KeyframeTimeline::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);

//	painter.setBrush(QBrush(QColor(255, 0, 0)));
//	painter.drawRect(0, 0, width(), height());

//	painter.setPen(QPen(Qt::black));
//	painter.drawText(QRect(3, 0, 32, 32), Qt::AlignLeft, tr("%1").arg(0));

	int width = this->width();
	int height = this->height();

	int ind_h = (height - (2*4)) / 3;

	float fulltime = (float)(m_end - m_start);

	int mid1y = 0 + ind_h + 4;
	int mid2y = 0 + (ind_h + 4) * 2;

	painter.setPen(Qt::gray);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0, 0, width-1, height-1);
	painter.drawLine(0, mid1y - 2, width - 1, mid1y - 2);
	painter.drawLine(0, mid2y - 2, width - 1, mid2y - 2);

	int frames = m_frames.size();
	for (int i=0; i < frames; i++)
	{
		int x = ((float)(m_frames[i]) / fulltime) * (width - 10);

		painter.setBrush(Qt::yellow);
		painter.setPen(Qt::black);
		painter.drawRect(x, 0, 8, ind_h);
	}

	for (int i=0; i < frames; i++)
	{
		int x = ((float)(m_frames[i]) / fulltime) * (width - 10);

		painter.setBrush(Qt::yellow);
		painter.setPen(Qt::black);
		painter.drawRect(x, 0 + ind_h + 4, 8, ind_h);
	}


	for (int i=0; i < frames; i++)
	{
		int x = ((float)(m_frames[i]) / fulltime) * (width - 10);

		painter.setBrush(Qt::yellow);
		painter.setPen(Qt::black);
		painter.drawRect(x, 0 + (ind_h + 4) * 2, 8, ind_h);
	}


	painter.end();
}