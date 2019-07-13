#ifndef NET_H
#define NET_H

#include <vector>
#include "db_obj.h"
#include "db_pin.h"
#include "db_cell.h"
#include "Wire.h"

namespace libs {
	class Net : public db_obj {
	public:
		type getType() { return type::NET; }
		void init();
		// net metods
	public:
		std::string name;
		std::vector<Wire> wires;
		std::vector<libs::db_pin> pins;
		std::vector<libs::db_cell> cells;
	};

	extern std::vector<Net> nets;
	extern std::vector<Net> created_nets;
}

#endif