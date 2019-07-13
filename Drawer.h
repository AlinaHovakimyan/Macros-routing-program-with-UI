#ifndef DRAWER_H
#define DRAWER_H

#include "db_obj.h"
#include "db_cell.h"
#include "Net.h"
#include <QtWidgets/QMainWindow>
#include <QPainter>
#include <QWidget>

class Renderer: public QWidget {
public:
	Renderer(QMainWindow* window);
	Renderer(QWidget* widget);
	void draw(db_obj* obj);
	void drawCell(libs::db_cell* obj);
	void drawNet(libs::Net net);
	void drawLines(const std::vector <std::vector<Wire>>& lines);
	bool refreshWindow() { return true; };
	QWidget* getWindow() { return m_centralWidget; };
protected:
	virtual void paintEvent(QPaintEvent *);

private:
	QMainWindow * m_window;
	QWidget * m_centralWidget;
	libs::db_cell * m_cell;
	bool m_linesIsSet;
	std::list<libs::Net> m_nets;
	std::vector <std::vector<Wire>> m_lines;
};

#endif
