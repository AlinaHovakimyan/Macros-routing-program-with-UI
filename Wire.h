#ifndef WIRE_H
#define WIRE_H

#include "db_obj.h"
#include "Point.h"

struct Wire : public db_obj {
	enum orient {
		HORIZONTAL,
		VERTICAL,
	};
	int x0;
	int y0;
	int x1;
	int y1;

	orient m_orient;
	int m_x;
	int m_y;
	Point m_startPoint;
	Point m_endPoint;

	Wire(int xs = 0, int ys = 0, int xe = 0, int ye = 0) :
		x0(xs), y0(ys), x1(xe), y1(ye) {};
	Wire(orient orientation, int coord);
	type getType() { return type::LINE; }
	bool operator==(const Wire& second) {
		return (m_startPoint.x == second.m_startPoint.x)
			&& (m_startPoint.y == second.m_startPoint.y)
			&& (m_endPoint.x == second.m_endPoint.x)
			&& (m_endPoint.y == second.m_endPoint.y);
	};
	bool contains(const Wire& second) const {
		if (m_orient != second.m_orient) {
			return false;
		}
		if (m_orient == HORIZONTAL) {
			if (m_startPoint.y != second.m_startPoint.y) {
				return false;
			}
			Point startPoint = m_startPoint.x < m_endPoint.x ? m_startPoint : m_endPoint;
			Point endPoint = m_startPoint.x > m_endPoint.x ? m_startPoint : m_endPoint;
			Point secondStartPoint = second.m_startPoint;
			//Point secondStartPoint = second.m_startPoint.x < second.m_endPoint.x ? second.m_startPoint : second.m_endPoint;
			//Point secondEndPoint = second.m_startPoint.x > second.m_endPoint.x ? second.m_startPoint : second.m_endPoint;

			if (startPoint.x <= secondStartPoint.x && endPoint.x >= secondStartPoint.x) {
				return true;
			}
			// at first wire have orientation and start point but no endPoint
			/*
			if (startPoint.x <= secondEndPoint.x && endPoint.x >= secondEndPoint.x) {
				return true;
			}
			*/
		}
		else if (m_orient == VERTICAL) {
			if (m_startPoint.x != second.m_startPoint.x) {
				return false;
			}
			Point startPoint = m_startPoint.y < m_endPoint.y ? m_startPoint : m_endPoint;
			Point endPoint = m_startPoint.y > m_endPoint.y ? m_startPoint : m_endPoint;
			Point secondStartPoint = second.m_startPoint;
			//Point secondStartPoint = second.m_startPoint.y < second.m_endPoint.y ? second.m_startPoint : second.m_endPoint;
			//Point secondEndPoint = second.m_startPoint.y > second.m_endPoint.y ? second.m_startPoint : second.m_endPoint;

			if (startPoint.y <= secondStartPoint.y && endPoint.y >= secondStartPoint.y) {
				return true;
			}
			/*
			if (startPoint.y <= secondEndPoint.y && endPoint.y >= secondEndPoint.y) {
				return true;
			}
			*/
		}
	}
};
#endif
