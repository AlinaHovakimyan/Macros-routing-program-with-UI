#ifndef CELL_H
#define CELL_H

#include <QtWidgets/QMainWindow>
#include "Declarations.h"
#include "IDrawableObj.h"
#include "db_cell.h"
#include "Net.h"
#include "db_pin.h"
#include "Wire.h"
#include "Point.h"
#include "Drawer.h"

namespace libs_ui {
	class Cell : public QWidget {
	public:
		Cell(QMainWindow* parent, libs::db_cell* libCell);
		void draw();
		static void draw(libs::db_cell* libCell, Renderer* window);
		~Cell();
	protected:
		virtual void paintEvent(QPaintEvent *);
	private:
		// not a member couse all cells must have the same color;
		static Color color;
		libs::db_cell* m_cell;
	};

	class Net : public IDrawableObj {
	public:
		static void draw(libs::Net* net, Renderer* window);
	private:
		static Color color;
		libs::Net m_net;
	};

	class Pin : public IDrawableObj {
	public:
		static void draw(libs::db_pin* libPin, Renderer* window);
	private:
		static Color color;
	};

	class PointUI : public IDrawableObj {
	public :
		static void draw(Point* point, Renderer* window);
	private:
		static Color color;
	};

	class LineUI : public IDrawableObj {
	public:
		static void draw(Wire* line, Renderer* window);
	protected:
		virtual void paintEvent(QPaintEvent *);
	private:
		static Color color;
	};
}

#endif