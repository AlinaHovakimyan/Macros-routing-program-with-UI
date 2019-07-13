#ifndef MST_H
#define MST_H

#include "db_pin.h"
#include <vector>

namespace MSTCreator {
	std::vector<libs::db_pin> MST(std::vector<libs::db_pin> pins);
};
#endif
