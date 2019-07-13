#include <QWidget>

#include "Drawer.h"
#include "Exception.h"
#include "Definitions.h"
#include "Cell.h"

Renderer::Renderer(QMainWindow* window)
	:QWidget(window)
{
	m_window = window;
	m_centralWidget = new QWidget();
	m_centralWidget->setMinimumSize(5000, 5000);
	m_centralWidget->setStyleSheet("background-color: black;");
	m_linesIsSet = false;
	//m_net = libs::Net();
	//  ete fonin ketikner dnelu anhrajeshtutyun arajana
	//QPalette pallet;
	//QBrush brush(Qt::black, Qt::Dense7Pattern);
	//pallet.setBrush(QPalette::Background, brush);
	//window->setPalette(pallet);
}

Renderer::Renderer(QWidget * widget)
	:QWidget(widget)
{
	m_centralWidget = widget;
	m_window = nullptr;
	setMinimumSize(1000, 1000);
	m_centralWidget->setStyleSheet("background-color: black;");
	m_linesIsSet = false;
	//m_net = libs::Net();
}

void Renderer::draw(db_obj * obj)
{
	switch (obj->getType())
	{
	case type::CELL:
	{
		//libs_ui::Cell::draw(static_cast<libs::sdb_cell*>(obj), this);
		drawCell(static_cast<libs::db_cell*>(obj));
		break;
	}
	case type::LINE:
	{
		libs_ui::LineUI::draw(static_cast<Wire*>(obj), this);
		break;
	}
	case type::NET:
	{
		libs_ui::Net::draw(static_cast<libs::Net*>(obj), this);
		break;
	}
	case type::PIN:
	{
		libs_ui::Pin::draw(static_cast<libs::db_pin*>(obj), this);
		break;
	}
	case type::POINT:
	{
		libs_ui::PointUI::draw(static_cast<Point*>(obj), this);
		break;
	}
	default:
	{
		throw ExceptionError("Can't draw object, because type is incorrect.");
		break;
	}
	}
}


#include <iostream>
#include "db_cell.h"

void Renderer::paintEvent(QPaintEvent *)
{
	QPainter p;
	p.begin(this);
	//p.begin(m_centralWidget);
	std::vector<libs::db_cell>::iterator i = libs::cells.begin();
	for (; i != libs::cells.end(); ++i) {
		QRect rect = QRect(i->xl, i->yl, (i->xh - i->xl), (i->yh - i->yl));
		p.setRenderHint(QPainter::Antialiasing);
		p.setPen(Qt::white);
		//p.fillRect(rect, Qt::gray);
		//p.setStyle()
		QBrush brush(Qt::white, Qt::DiagCrossPattern);
		p.setBrush(brush);
		//p.setBackground(Qt::DiagCrossPattern);
		p.drawRect(rect);
		p.setPen(Qt::blue);
		QFont font = p.font();
		font.setPixelSize(30);
		p.setFont(font);
		p.drawText((i->xl + i->xh)/2 - 5, (i->yl - 2), QString::fromStdString(i->name));
	}

	p.setPen(QPen(Qt::darkCyan, 3));
	if (m_linesIsSet) {
		std::vector<std::vector<Wire>>::const_iterator z = m_lines.begin();
		std::vector<Wire>::const_iterator j = z->begin();
		for (; z != m_lines.end(); ++z) {
			j = z->begin();
			for (; j != z->end(); ++j) {
				if ((j->m_endPoint.x != 0) && (j->m_endPoint.y != 0)) {
					p.drawLine(j->m_startPoint.x, j->m_startPoint.y, j->m_endPoint.x, j->m_endPoint.y);
				}
			}
		}
	}

	std::list<libs::Net>::iterator nets_i = m_nets.begin();
	for (; nets_i != m_nets.end(); ++nets_i) {
		if (!(*nets_i).wires.empty()) {
			p.setPen(QPen(Qt::cyan, 3));          // cyan==blue
			std::vector<Wire>::iterator j = (*nets_i).wires.begin();
			for (; j != (*nets_i).wires.end(); ++j) {
				p.drawLine(j->m_startPoint.x, j->m_startPoint.y, j->m_endPoint.x, j->m_endPoint.y);
			}
		}
	}

	QPen linepen(Qt::red);
	linepen.setCapStyle(Qt::RoundCap);
	linepen.setWidth(8);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(linepen);
	for (nets_i = m_nets.begin(); nets_i != m_nets.end(); ++nets_i) {
		if (!(*nets_i).wires.empty()) {
			p.drawPoint((*nets_i).wires.at(0).m_endPoint.x, (*nets_i).wires.at(0).m_endPoint.y);
		}
	}
	p.end();
}

#include <QHBoxLayout>
#include <QLabel>

void Renderer::drawCell(libs::db_cell * libCell)
{
	m_cell = libCell;
}

void Renderer::drawNet(libs::Net net)
{
	m_nets.push_back(net);
}

void Renderer::drawLines(const std::vector<std::vector<Wire>>& lines)
{
	m_lines = lines;
	m_linesIsSet = true;
}
