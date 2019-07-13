#include "MST.h"
#include <queue>
#include <vector>
#include <list>


int getPointsDistance(const libs::db_pin& pin1, const libs::db_pin& pin2)
{
	return abs(pin1.x - pin2.x) + abs(pin1.y - pin2.y);
}

std::vector<libs::db_pin> MSTCreator::MST(std::vector<libs::db_pin> pins)
{
	std::vector<libs::db_pin> retMST;
	retMST.push_back(pins.at(0));
	pins.erase(pins.begin() + 0);

	std::vector<libs::db_pin>::iterator i = retMST.begin();
	std::vector<libs::db_pin>::iterator j = pins.begin();
	int distance = getPointsDistance(retMST.at(0), pins.at(0));
	int tempDistance = 0;
	std::vector<libs::db_pin>::iterator tempEndPointIterator = pins.begin() + 0;

	for (; i != retMST.end();) {
		if (pins.empty()) {
			return retMST;
		}
		j = pins.begin();
		distance = getPointsDistance(*i, pins.at(0));
		tempEndPointIterator = pins.begin() + 0;
		for (; j != pins.end(); ++j) {
			tempDistance = getPointsDistance(*i, *j);
			if (tempDistance < distance) {
				distance = tempDistance;
				tempEndPointIterator = j;
			}
		}
		i = retMST.insert(retMST.end(), *tempEndPointIterator);
		j = pins.erase(tempEndPointIterator);
	}
	return retMST;
}
