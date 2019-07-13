#include "db_cell.h"

namespace libs {

	std::vector<db_cell> cells;
	void db_cell::init() {
		name = "";
		xl = 0;
		yl = 0;
		xh = 0;
		yh = 0;
		pins.clear();
	}

}