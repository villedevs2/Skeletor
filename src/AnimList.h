#pragma once

#include <QtGui>
#include <QListWidget>
#include <vector>

using namespace std;


class AnimListDelegate : public QStyledItemDelegate
{
public:
	AnimListDelegate(QObject* parent=0) : QStyledItemDelegate(parent) {}
	~AnimListDelegate() {}
	
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QRect r = option.rect;

		if (option.state & QStyle::State_Selected)
		{
			//painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
			QLinearGradient gradientSelected(r.left(),r.top(),r.left(),r.height()+r.top());
			gradientSelected.setColorAt(0.0, QColor::fromRgb(119,213,247));
			gradientSelected.setColorAt(0.9, QColor::fromRgb(27,134,183));
			gradientSelected.setColorAt(1.0, QColor::fromRgb(0,120,174));
			painter->setBrush(gradientSelected);
			painter->drawRect(r);
		}

		QString title = index.data(Qt::DisplayRole).toString();
		QString start = "Start: " + index.data(Qt::UserRole + 1).toString();
		QString end = "End: " + index.data(Qt::UserRole + 2).toString();

		QPen white(QColor::fromRgb(255, 255, 255), 1, Qt::SolidLine);
		QPen black(QColor::fromRgb(0,0,0), 1, Qt::SolidLine);

		painter->setFont(QFont("Lucida Grande", 8, QFont::Normal));
			
		if (option.state & QStyle::State_Selected)
			painter->setPen(white);
		else
			painter->setPen(black);

		painter->drawText(r, Qt::AlignBottom|Qt::AlignLeft, title);

		int tab_width = ((r.width() * 1) / 5) + 20;
		QRect start_rect = QRect(r.left() + r.width() - (tab_width * 2), r.top(), tab_width, r.height());
		QRect end_rect = QRect(r.left() + r.width() - tab_width, r.top(), tab_width, r.height());

		painter->drawText(start_rect, Qt::AlignBottom|Qt::AlignLeft, start);
		painter->drawText(end_rect, Qt::AlignBottom|Qt::AlignLeft, end);
	}

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(400, 24);
	}
};



class AnimList : public QWidget
{
	Q_OBJECT

public:
	struct AnimListEntry
	{
		std::string name;
		int start;
		int end;
	};

	AnimList(QWidget *parent);
	~AnimList();

	void getAnimations(vector<AnimListEntry>& animations);

signals:

public slots:
	void onAdd();
	void onRemove();

private:
	QListWidget* m_list;
	QLineEdit* m_name_text;
	QSpinBox* m_start;
	QSpinBox* m_end;
	QPushButton* m_add_button;
	QGroupBox* m_addgroup;
	QPushButton* m_remove_button;
};