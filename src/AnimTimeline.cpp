#include "AnimTimeline.h"

AnimTimeline::AnimTimeline(QWidget *parent) : QWidget(parent)
{
	m_slider = new QSlider(Qt::Horizontal, parent);
	m_slider->setMinimumWidth(this->width());
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setTickInterval(100);
	m_slider->setMinimum(0);
	m_slider->setMaximum(1000);

	m_timeline = new KeyframeTimeline(parent);

	m_frame_text = new QLineEdit();
	m_frame_text->setMaxLength(10);
	m_frame_text->setReadOnly(false);
	m_frame_text->setAlignment(Qt::AlignHCenter);
	m_frame_text->insert("0");
	m_frame_text->setMaximumWidth(70);

	m_prev = new QPushButton("<<");
	m_prev->setMaximumWidth(32);
	
	m_next = new QPushButton(">>");
	m_next->setMaximumWidth(32);


	m_group = new QGroupBox("Animation Timeline", this);
	QGridLayout *group_layout = new QGridLayout();
	group_layout->addWidget(m_slider, 0, 0, 1, 1);
	group_layout->addWidget(m_frame_text, 0, 1, 1, 2);
	group_layout->addWidget(m_timeline, 1, 0, 1, 1);
	group_layout->addWidget(m_prev, 1, 1, 1, 1);
	group_layout->addWidget(m_next, 1, 2, 1, 1);
	m_group->setLayout(group_layout);
	m_group->setMinimumWidth(1000);
	m_group->setMaximumWidth(1000);


	// connect signals
	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(setFrameSliderText(int)));

	connect(m_prev, SIGNAL(clicked()), this, SLOT(onPrevFrame()));
	connect(m_next, SIGNAL(clicked()), this, SLOT(onNextFrame()));

	connect(m_slider, SIGNAL(valueChanged(int)), this, SIGNAL(sliderChanged(int)));



	m_current_frame = 0;
}

AnimTimeline::~AnimTimeline()
{
}

void AnimTimeline::resetKeyframes()
{
	m_timeline->reset();

	m_current_frame = 0;
}

void AnimTimeline::insertKeyframe(int time)
{
	m_timeline->insert(time);
}

void AnimTimeline::setRange(int start, int end)
{
	m_timeline->setRange(start, end);

	m_slider->setMinimum(start);
	m_slider->setMaximum(end);
}

void AnimTimeline::setFrameSliderText(int value)
{
	m_frame_text->setText(tr("%1").arg(value));
}

void AnimTimeline::onPrevFrame()
{
	m_current_frame--;
	if (m_current_frame < 0)
		m_current_frame = 0;

	if (m_timeline->getNumFrames() > 0)
	{
		m_slider->setValue(m_timeline->getFrame(m_current_frame));
	}
	else
	{
		m_slider->setValue(0);
	}
}

void AnimTimeline::onNextFrame()
{
	m_current_frame++;

	int maxframes = m_timeline->getNumFrames();

	if (maxframes > 0)
	{
		if (m_current_frame > (maxframes-1))
			m_current_frame = (maxframes-1);

		int value = m_timeline->getFrame(m_current_frame);

		m_slider->setValue(value);
	}
	else
	{
		m_current_frame = 0;
		m_slider->setValue(0);
	}
}