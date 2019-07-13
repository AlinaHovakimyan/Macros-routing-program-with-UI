#ifndef POINT_H
#define POINT_H

#include <tuple>
#include "db_obj.h"

struct Point : public db_obj {
	int x;
	int y;
	Point(int x0 = 0, int y0 = 0);
	type getType() { return POINT; }
	friend bool operator<(const Point& lhs, const Point& rhs) {
		// std::tuple's lexicographic ordering does all the actual work for you
		// and using std::tie means no actual copies are made
		return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
	}
	bool operator==(const Point& other) {
		return ((x == other.x) && (y == other.y));
	}

	bool operator!=(const Point& other) {
		return (!operator==(other));
	}
};

#endif
