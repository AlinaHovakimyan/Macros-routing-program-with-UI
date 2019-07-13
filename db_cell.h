#ifndef SDB_CELL_H
#define SDB_CELL_H
#include <vector>

#include "db_obj.h"
#include "db_pin.h"

namespace libs {
	struct db_cell : public db_obj {
	public:
		db_cell() {};
		db_cell(std::string ind, unsigned int xleft, unsigned int yleft,
			unsigned int xright, unsigned int yright) : name(ind), xl(xleft), yl(yleft),
			xh(xright), yh(yright) {};
		void init();
		bool operator==(const db_cell& other) const { return this->name == other.name; }
	public:
		virtual type getType() { return type::CELL; }
	public:
		std::string name;
		unsigned int xl;
		unsigned int yl;
		unsigned int xh;
		unsigned int yh;
		std::vector<db_pin> pins;
	};

	extern std::vector<db_cell> cells;

}

#endif
