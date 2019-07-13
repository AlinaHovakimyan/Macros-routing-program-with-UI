#include "Cell.h"

Color libs_ui::Cell::color = Color::white;
Color libs_ui::LineUI::color = Color::blue;
Color libs_ui::Net::color = Color::green;
Color libs_ui::Pin::color = Color::red;
Color libs_ui::PointUI::color = Color::yellow;

libs_ui::Cell::~Cell() {}
void libs_ui::Cell::paintEvent(QPaintEvent *)
{
	QPainter p;
	p.begin(this);
	p.setPen(Qt::blue);
	p.setFont(QFont("Arial", 30));
	p.drawText(rect(), Qt::AlignCenter, "CELL");
	//p.drawLine(...);        // drawing code
	p.end();
}
;

#include <QHBoxLayout>
#include <QPushButton>
#include <iostream>
#include <QLabel>

namespace libs_ui {
	Cell::Cell(QMainWindow* parent, libs::db_cell * libCell) :
		QWidget(parent)
	{
		setFixedSize(100, 100);
		setGeometry(0, 0, 100, 100);
		QPalette pal = palette();

		// set black background
		pal.setColor(QPalette::Background, Qt::black);
		setAutoFillBackground(true);
		setPalette(pal);

		QLabel *label = new QLabel(this);
		QHBoxLayout *layout = new QHBoxLayout();
		//label->setText("Random String");
		layout->addWidget(label);
		this->setLayout(layout);
		//setLayout(layout);

		QHBoxLayout *layout1 = new QHBoxLayout();
		layout1->addWidget(this);


		parent->setCentralWidget(new QWidget);
		parent->centralWidget()->setLayout(layout1);

		m_cell = libCell;
	}

	void Cell::draw()
	{
		setUpdatesEnabled(true);
		showNormal();
		repaint();
	}

	void Cell::draw(libs::db_cell* libCell, Renderer* window) {
		std::cout << "cell drow" << std::endl;
		QRect rect = QRect(libCell->xl, libCell->yl, (libCell->xh - libCell->xl), (libCell->yh - libCell->yl));
	}

	void LineUI::draw(Wire* line, Renderer* window) {

	}

	void LineUI::paintEvent(QPaintEvent *)
	{
		QPainter p;
		p.begin(this);
		p.setPen(Qt::blue);
		p.setFont(QFont("Arial", 30));
		p.drawText(rect(), Qt::AlignCenter, "LINE");
		//p.drawLine(...);        // drawing code
		p.end();
	}

	void Net::draw(libs::Net* net, Renderer* window) {

	}

	void Pin::draw(libs::db_pin* pin, Renderer* window) {

	}

	void PointUI::draw(Point* point, Renderer* window) {

	}
}
