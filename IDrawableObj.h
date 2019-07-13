#ifndef IDRAWABLE_OBJ_H
#define IDRAWABLE_OBJ_H

#include <QWidget>
#include <QtWidgets/QMainWindow>
#include <QPainter>
#include <QWidget>

class IDrawableObj : public QWidget {
public:
	virtual void draw() = 0;
	//virtual ~IDrawableObj() = 0;
};

#endif