#if 0
#include "AnimList.h"



AnimList::AnimList(QWidget *parent) : QWidget(parent)
{
	m_list = new QListWidget();
	m_list->setMinimumWidth(420);
	m_list->setMinimumHeight(500);
	m_list->setMaximumHeight(500);

	m_list->setItemDelegate(new AnimListDelegate(m_list));
	m_list->showMaximized();


	m_name_text = new QLineEdit();
	m_name_text->setMaxLength(40);
	m_name_text->setReadOnly(false);
	m_name_text->setAlignment(Qt::AlignLeft);
	m_name_text->insert("");
	m_name_text->setMaximumWidth(150);

	m_start = new QSpinBox();
	m_start->setRange(0, 1000000);
	m_start->setAlignment(Qt::AlignLeft);
	m_start->setMaximumWidth(70);

	m_end = new QSpinBox();
	m_end->setRange(0, 1000000);
	m_end->setAlignment(Qt::AlignLeft);
	m_end->setMaximumWidth(70);


	m_add_button = new QPushButton("Add");
	m_add_button->setMaximumWidth(32);

	m_remove_button = new QPushButton("Remove");
	m_remove_button->setMaximumWidth(48);


	m_addgroup = new QGroupBox("Animation");
	QGridLayout *group_layout = new QGridLayout();
	group_layout->addWidget(m_name_text, 0, 0, 1, 1);
	group_layout->addWidget(m_start, 0, 1, 1, 1);
	group_layout->addWidget(m_end, 0, 2, 1, 1);
	group_layout->addWidget(m_add_button, 0, 3, 1, 1);
	group_layout->addWidget(m_remove_button, 0, 4, 1, 1);
	m_addgroup->setLayout(group_layout);


	QVBoxLayout* master_layout = new QVBoxLayout(this);
	master_layout->setSpacing(0);
	master_layout->setMargin(0);
	master_layout->addWidget(m_list);
	master_layout->addWidget(m_addgroup);
	master_layout->addStretch(1);



	connect(m_add_button, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(m_remove_button, SIGNAL(clicked()), this, SLOT(onRemove()));
}

AnimList::~AnimList()
{
}

void AnimList::onAdd()
{
	QString text = m_name_text->text();

	if (text.length() > 0)
	{
		QListWidgetItem* item = new QListWidgetItem();
		item->setData(Qt::DisplayRole, m_name_text->text());
		item->setData(Qt::UserRole + 1, m_start->value());
		item->setData(Qt::UserRole + 2, m_end->value());

		m_list->addItem(item);

		m_name_text->setText("");
		m_start->setValue(0);
		m_end->setValue(0);
	}
	else
	{
		QMessageBox::critical(this, "Error", "Animation name is empty.", QMessageBox::Ok, QMessageBox::Ok);
	}
}

void AnimList::onRemove()
{
	QListWidgetItem* item = m_list->currentItem();
	if (item)
	{
		m_list->takeItem(m_list->currentRow());
		delete item;
	}
}

void AnimList::getAnimations(vector<AnimListEntry> &animations)
{
	int num_anims = m_list->count();
	for (int i=0; i < num_anims; i++)
	{
		QListWidgetItem* item = m_list->item(i);

		AnimListEntry entry;
		entry.name = item->data(Qt::DisplayRole).toString().toStdString();
		entry.start = item->data(Qt::UserRole + 1).toInt();
		entry.end = item->data(Qt::UserRole + 2).toInt();

		animations.push_back(entry);
	}
}
#endif