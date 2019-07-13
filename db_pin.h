#ifndef DB_PIN_H
#define DB_PIN_H

#include "db_obj.h"
#include <string>

namespace libs {
	class db_cell;
	class db_pin : public db_obj {
	public:
		type getType() { return type::PIN; }
		bool operator==(const db_pin& other) const { return this->name == other.name; }
		std::string name;
		int x;
		int y;
		libs::db_cell* parent;
	};
}

inline bool operator<(const libs::db_pin& first, const libs::db_pin& second) { return (first.x + first.y) < (second.x + second.y); };

#endif