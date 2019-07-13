#include "Net.h"

namespace libs {

	std::vector<Net> nets;
	std::vector<Net> created_nets;
	void Net::init() {
		name = "";
		pins.clear();
		wires.clear();
	}

}