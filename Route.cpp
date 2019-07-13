#include "Definitions.h"
#include "Route.h"
#include "Wire.h"
#include "Point.h"
#include "MST.h"
#include <thread>

std::vector <std::vector<Wire>> m_lines;

Router::Router()
{
	m_lines.resize(1);
	m_linesIndex = 0;
	m_x_min = 0;
	m_x_max = 0;
}

void Router::route(std::vector<libs::Net> nets)
{
	//if (!m_lines.empty()) {
	//	m_lines.clear();
	//	m_lines.resize(1);
	//}
	std::vector<libs::Net>::iterator i = libs::nets.begin();
	libs::Net net;
	std::vector<libs::Net> tempNetVector;
	for (; i != libs::nets.end(); ++i) {
		std::vector<libs::db_pin> sortedPins = MSTCreator::MST(i->pins);
		std::vector<libs::db_pin>::iterator j = sortedPins.begin();
		for (; j != sortedPins.end() - 1; ++j) {
			net = route(*j, *(j + 1));
			tempNetVector.push_back(net);
		}

		std::vector<libs::Net>::iterator i = tempNetVector.begin();
		for (; i != tempNetVector.end(); ++i) {
			libs::created_nets.push_back(*i);
		}
	}
}

// router that gets two pins
libs::Net Router::route(libs::db_pin first, libs::db_pin second)
{
	initRouter();
	createSourcePoint(first);
	createTargetPoint(second);
	calculateRoutingArea();

	bool continueRoute = true;
	while (continueRoute) {
		std::thread thread1(&Router::createWiresOfFirstPin, this);
		std::thread thread2(&Router::createWiresOfSecondPin, this);
		thread1.join();
		thread2.join();

		m_firstPoints.clear();
		m_secondPoints.clear();
		++m_linesIndex;
		m_lines.resize(m_linesIndex + 1);
		std::list<Wire*>::iterator first_j = m_firstWires.begin();
		std::list<Wire*>::iterator second_j = m_secondWires.begin();

		while ((first_j != m_firstWires.end()) && continueRoute) {
			for (second_j = m_secondWires.begin(); second_j != m_secondWires.end() && continueRoute; ++second_j) {
				if (((*first_j)->m_orient != (*second_j)->m_orient) && crossed(*first_j, *second_j)) {
					continueRoute = false;
				}
			}
			++first_j;
		}
	}
	return	m_net;
}

bool Router::crossed(Wire* first, Wire* second)
{
	Point* crossPoint = nullptr;
	if (first->m_orient == Wire::orient::HORIZONTAL) {
		crossPoint = new Point(second->m_x, first->m_y);
	}
	else {
		crossPoint = new Point(first->m_x, second->m_y);
	}
	
	if (inPermissibleArea(crossPoint, first, second) && ((*crossPoint != m_firstPoint) && (*crossPoint != m_secondPoint))) {
		Point endPoint = Point(crossPoint->x, crossPoint->y);
		first->m_endPoint = endPoint;
		endPointsMap1.insert(std::make_pair(endPoint, *first));
		m_firstPoints.push_back(endPoint);
		m_lines[m_linesIndex].push_back(*first);
		second->m_endPoint = endPoint;
		m_secondPoints.push_back(endPoint);
		endPointsMap2.insert(std::make_pair(endPoint, *second));
		m_lines[m_linesIndex].push_back(*second);

		m_net.wires.push_back(*first);
		m_net.wires.push_back(*second);
		Point startPoint1 = first->m_startPoint;
		Point startPoint2 = second->m_startPoint;
		Wire h;
		Wire v;

		while (((startPoint1 != m_firstPoint) && (startPoint1 != m_secondPoint)) 
				|| ((startPoint2 != m_firstPoint) && (startPoint2 != m_secondPoint))) {
			if (endPointsMap1.find(startPoint1) != endPointsMap1.end()) {
				h = endPointsMap1.find(startPoint1)->second;
			}
			else {
				h = endPointsMap2.find(startPoint1)->second;
			}
			if (endPointsMap1.find(startPoint2) != endPointsMap1.end()) {
				v = endPointsMap1.find(startPoint2)->second;
			}
			else {
				v = endPointsMap2.find(startPoint2)->second;
			}
			m_net.wires.push_back(h);
			m_net.wires.push_back(v);
			startPoint1 = v.m_startPoint;
			startPoint2 = h.m_startPoint;
		}
		return true;
	}
	return false;
}

#include <iostream>

libs::db_cell* Router::findNearestCellInRange(Point * sourcePoint, Point * endPoint)
{
	std::vector<libs::db_cell>::iterator i = libs::cells.begin();
	unsigned int absDiff = UINT_MAX;
	libs::db_cell* tempCellPtr = nullptr;
	if (sourcePoint->x == endPoint->x) {
		for (; i != libs::cells.end(); ++i) {
			if ((sourcePoint->y < endPoint->y && ((*i).yl < endPoint->y && (*i).yh > sourcePoint->y)) || (sourcePoint->y > endPoint->y && ((*i).yh > endPoint->y && (*i).yl < sourcePoint->y))) {
				if ((i->xl <= sourcePoint->x && sourcePoint->x <= i->xh) && ((abs((int)(sourcePoint->y - (*i).yl)) < absDiff || (abs((int)(sourcePoint->y - (*i).yh)) < absDiff))))
				{
					tempCellPtr = &*i;
					if (abs((int)(sourcePoint->y - (*i).yl)) <= (abs((int)(sourcePoint->y - (*i).yh)))) {
						absDiff = abs((int)(sourcePoint->y - (*i).yl));
					}
					else {
						absDiff = abs((int)(sourcePoint->y - (*i).yh));
					}
				}
			}
		}
	}
	else if (sourcePoint->y == endPoint->y) {
		for (; i != libs::cells.end(); ++i) {
			if ((sourcePoint->x < endPoint->x && ((*i).xl < endPoint->x && ((*i).xh > sourcePoint->x))) || (sourcePoint->x > endPoint->x && ((*i).xh > endPoint->x && ((*i).xl < sourcePoint->x)))) {
				if (/*(sourcePoint->x != ((*i).xh + grid_offset) && (sourcePoint->x != ((*i).xl) - grid_offset)) && */(i->yl <= sourcePoint->y && sourcePoint->y <= i->yh) && ((abs((int)(sourcePoint->x - (*i).xl)) < absDiff || (abs((int)(sourcePoint->x - (*i).xh)) < absDiff))))
				{
					tempCellPtr = &*i;
					if (abs((int)(sourcePoint->x - (*i).xl)) <= (abs((int)(sourcePoint->x - (*i).xh)))) {
						absDiff = abs((int)(sourcePoint->x - (*i).xl));
					}
					else {
						absDiff = abs((int)(sourcePoint->x - (*i).xh));
					}
				}
			}
		}
	}
	return tempCellPtr;
}

bool Router::inPermissibleArea(Point* crossPoint, Wire* first, Wire* second)
{
	Point horizontalStart;
	Point verticalStart;
	if (first->m_orient == Wire::orient::HORIZONTAL) {
		crossPoint = new Point(second->m_x, first->m_y);
		horizontalStart = first->m_startPoint;
		verticalStart = second->m_startPoint;
	}
	else {
		crossPoint = new Point(first->m_x, second->m_y);
		horizontalStart = second->m_startPoint;
		verticalStart = first->m_startPoint;
	}

	Point endPoint;
	Point endPoint2;
	bool firstHaveEnd = false;
	bool secondHaveEnd = false;

	libs::db_cell* firstCellPtr = findNearestCellInRange(&first->m_startPoint, crossPoint);
	libs::db_cell* secondCellPtr = findNearestCellInRange(&second->m_startPoint, crossPoint);
	if (firstCellPtr != nullptr) {
		if (first->m_orient == Wire::orient::HORIZONTAL) {
			int new_x = (horizontalStart.x > firstCellPtr->xh) ? firstCellPtr->xh + grid_offset : firstCellPtr->xl - grid_offset;
			endPoint = Point(new_x, crossPoint->y);
		}
		else {
			int new_y = (verticalStart.y > firstCellPtr->yh) ? firstCellPtr->yh + grid_offset : firstCellPtr->yl - grid_offset;
			endPoint = Point(crossPoint->x, new_y);
		}
		first->m_endPoint = endPoint;
		m_firstPoints.push_back(endPoint);
		endPointsMap1.insert(std::make_pair(endPoint, *first));
		m_lines[m_linesIndex].push_back(*first);
		firstHaveEnd = true;
		if (first->m_startPoint.x == crossPoint->x) {
			endPoint = cultivateEndPointOfVertical(horizontalStart, verticalStart, crossPoint, second);
		} 
		else  if (first->m_startPoint.y == crossPoint->y) {
			endPoint = cultivateEndPointOfHorizontal(horizontalStart, verticalStart, crossPoint, first);
		}
		first->m_endPoint = endPoint;
		m_firstPoints.push_back(endPoint);
		endPointsMap1.insert(std::make_pair(endPoint, *first));
		m_lines[m_linesIndex].push_back(*first);
	}
	if (secondCellPtr != nullptr) {
		if (second->m_orient == Wire::orient::HORIZONTAL) {
			int new_x = (horizontalStart.x > secondCellPtr->xh) ? secondCellPtr->xh + grid_offset : secondCellPtr->xl - grid_offset;
			endPoint = Point(new_x, crossPoint->y);
		}
		else {
			int new_y = (verticalStart.y > secondCellPtr->yh) ? secondCellPtr->yh + grid_offset : secondCellPtr->yl - grid_offset;
			endPoint = Point(crossPoint->x, new_y);
		}
		second->m_endPoint = endPoint;
		m_secondPoints.push_back(endPoint);
		endPointsMap2.insert(std::make_pair(endPoint, *second));
		m_lines[m_linesIndex].push_back(*second);
		secondHaveEnd = true;
		if (second->m_startPoint.x == crossPoint->x) {
			endPoint = cultivateEndPointOfVertical(horizontalStart, verticalStart, crossPoint, second);
		}
		else  if (second->m_startPoint.y == crossPoint->y) {
			endPoint = cultivateEndPointOfHorizontal(horizontalStart, verticalStart, crossPoint, first);
		}
		second->m_endPoint = endPoint;
		m_secondPoints.push_back(endPoint);
		endPointsMap2.insert(std::make_pair(endPoint, *second));
		m_lines[m_linesIndex].push_back(*second);
	}
	if (firstHaveEnd && !secondHaveEnd) {
		libs::db_cell* finded_cell;
		libs::db_cell* finded_cell2;
		if (second->m_startPoint.x == crossPoint->x) {
			if (second->m_startPoint.y < crossPoint->y) {
				Point* tempPointToCorner = new Point(second->m_startPoint.x, m_y_min);
				Point* tempPointToCorner2 = new Point(second->m_startPoint.x, m_y_max);
				finded_cell = findNearestCellInRange(&second->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&second->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(second->m_startPoint.x, m_y_min);
				}
				if (finded_cell2 != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint2 = Point(second->m_startPoint.x, m_y_max);
				}
			}
			else if (second->m_startPoint.y > crossPoint->y) {
				Point* tempPointToCorner = new Point(second->m_startPoint.x, m_y_max);
				Point* tempPointToCorner2 = new Point(second->m_startPoint.x, m_y_min);
				finded_cell = findNearestCellInRange(&second->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&second->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(second->m_startPoint.x, m_y_max);
				}
				if (finded_cell2 != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(second->m_startPoint.x, m_y_min);
				}
			}
		}
		else if (second->m_startPoint.y == crossPoint->y) {
			if (second->m_startPoint.x < crossPoint->x) {
				Point* tempPointToCorner = new Point(m_x_min, second->m_startPoint.y);
				Point* tempPointToCorner2 = new Point(m_x_max, second->m_startPoint.y);
				finded_cell = findNearestCellInRange(&second->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&second->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(m_x_min, second->m_startPoint.y);
				}
				if (finded_cell2 != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint2 = Point(m_x_max, second->m_startPoint.y);
				}
			}
			else if (second->m_startPoint.x > crossPoint->x) {
				Point* tempPointToCorner = new Point(m_x_max, second->m_startPoint.y);
				Point* tempPointToCorner2 = new Point(m_x_min, crossPoint->y);
				finded_cell = findNearestCellInRange(&second->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&second->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(m_x_max, second->m_startPoint.y);
				}
				if (finded_cell2 != nullptr) {
					if (second->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint2 = Point(m_x_min, second->m_startPoint.y);
				}
			}
		}
		second->m_endPoint = endPoint;
		m_secondPoints.push_back(endPoint);
		endPointsMap2.insert(std::make_pair(endPoint, *second));
		m_lines[m_linesIndex].push_back(*second);

		second->m_endPoint = endPoint2;
		m_secondPoints.push_back(endPoint2);
		endPointsMap2.insert(std::make_pair(endPoint2, *second));
		m_lines[m_linesIndex].push_back(*second);
	}
	else if (!firstHaveEnd && secondHaveEnd) {
		libs::db_cell* finded_cell;
		libs::db_cell* finded_cell2;
		if (first->m_startPoint.x == crossPoint->x) {
			if (first->m_startPoint.y < crossPoint->y) {
				Point* tempPointToCorner = new Point(first->m_startPoint.x, m_y_min);
				Point* tempPointToCorner2 = new Point(first->m_startPoint.x, m_y_max);
				finded_cell = findNearestCellInRange(&first->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&first->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(first->m_startPoint.x, m_y_min);
				}
				if (finded_cell2 != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint2 = Point(first->m_startPoint.x, m_y_max);
				}
			}
			else if (first->m_startPoint.y > crossPoint->y) {
				Point* tempPointToCorner = new Point(first->m_startPoint.x, m_y_max);
				Point* tempPointToCorner2 = new Point(first->m_startPoint.x, m_y_min);
				finded_cell = findNearestCellInRange(&first->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&first->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(first->m_startPoint.x, m_y_max);
				}
				if (finded_cell2 != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint2 = Point(first->m_startPoint.x, m_y_min);
				}
			}
		}
		else if (first->m_startPoint.y == crossPoint->y) {
			if (first->m_startPoint.x < crossPoint->x) {
				Point* tempPointToCorner = new Point(m_x_min, first->m_startPoint.y);
				Point* tempPointToCorner2 = new Point(m_x_max, first->m_startPoint.y);
				finded_cell = findNearestCellInRange(&first->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&first->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(m_x_min, first->m_startPoint.y);
				}
				if (finded_cell2 != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint2 = Point(m_x_max, first->m_startPoint.y);
				}
			}
			else if (first->m_startPoint.x > crossPoint->x) {
				Point* tempPointToCorner = new Point(m_x_max, first->m_startPoint.y);
				Point* tempPointToCorner2 = new Point(m_x_min, first->m_startPoint.y);
				finded_cell = findNearestCellInRange(&first->m_startPoint, tempPointToCorner);
				finded_cell2 = findNearestCellInRange(&first->m_startPoint, tempPointToCorner2);
				if (finded_cell != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
						endPoint = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
						endPoint = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(m_x_max, first->m_startPoint.y);
				}
				if (finded_cell2 != nullptr) {
					if (first->m_orient == Wire::orient::HORIZONTAL) {
						int new_x = (horizontalStart.x > finded_cell2->xh) ? finded_cell2->xh + grid_offset : finded_cell2->xl - grid_offset;
						endPoint2 = Point(new_x, crossPoint->y);
					}
					else {
						int new_y = (verticalStart.y > finded_cell2->yh) ? finded_cell2->yh + grid_offset : finded_cell2->yl - grid_offset;
						endPoint2 = Point(crossPoint->x, new_y);
					}
				}
				else {
					endPoint = Point(m_x_min, first->m_startPoint.y);
				}
			}
		}
		first->m_endPoint = endPoint;
		m_firstPoints.push_back(endPoint);
		endPointsMap1.insert(std::make_pair(endPoint, *first));
		m_lines[m_linesIndex].push_back(*first);


		first->m_endPoint = endPoint2;
		m_firstPoints.push_back(endPoint2);
		endPointsMap1.insert(std::make_pair(endPoint2, *first));
	}
	if (firstHaveEnd || secondHaveEnd) {
 		return false;
	}
	else {
		return true;
	}
}

bool Router::gridIsFree(const Wire * grid)
{

	std::vector<libs::Net>::iterator i = libs::created_nets.begin();
	std::vector<Wire>::iterator j;
	for (; i != libs::created_nets.end(); ++i) {
		for (j = i->wires.begin(); j != i->wires.end(); ++j) {
			if (j->contains(*grid) || grid->contains(*j)) {
				return false;
			}
		}
	}
	return true;
}

void Router::createWiresOfFirstPin()
{
	Wire* AH = nullptr;
	Wire* AV = nullptr;
	m_firstWires.clear();
	std::vector<Point>::iterator first_i = m_firstPoints.begin();
	for (; first_i != m_firstPoints.end(); ++first_i) {
		if (endPointsMap1.empty()) {
			AH = new Wire(Wire::orient::HORIZONTAL, first_i->y);
			AH->m_startPoint = *first_i;
			AV = new Wire(Wire::orient::VERTICAL, first_i->x);
			AV->m_startPoint = *first_i;
			if (gridIsFree(AH)) {
				m_firstWires.push_back(AH);
			} else {
					AH = new Wire(Wire::orient::HORIZONTAL, first_i->y - grid_offset);
					Point createdNewPoint(first_i->x, first_i->y - grid_offset);
					AH->m_startPoint = createdNewPoint;
					m_firstWires.push_back(AH);

					Wire* tempWire = new Wire(Wire::orient::VERTICAL, first_i->x);
					tempWire->m_endPoint = createdNewPoint;
					tempWire->m_startPoint = *first_i;
					endPointsMap1.insert(std::make_pair(createdNewPoint, *tempWire));
					m_lines[m_linesIndex].push_back(*tempWire);
					*first_i = createdNewPoint;

					// stex irakanum petqa 2 piner@ kpcnox net sarqes qces neteri mej nor kam verjum inch vor stuguma petq
				}
			if (gridIsFree(AV)) {
				m_firstWires.push_back(AV);
			}
			else {
				AV = new Wire(Wire::orient::VERTICAL, first_i->x - grid_offset);
				Point createdNewPoint(first_i->x - grid_offset, first_i->y);
				AV->m_startPoint = createdNewPoint;
				m_firstWires.push_back(AV);

				Wire* tempWire = new Wire(Wire::orient::HORIZONTAL, first_i->y);
				tempWire->m_endPoint = createdNewPoint;
				tempWire->m_startPoint = *first_i;
				endPointsMap1.insert(std::make_pair(createdNewPoint, *tempWire));
				m_lines[m_linesIndex].push_back(*tempWire);
				*first_i = createdNewPoint;

				// stex irakanum petqa 2 piner@ kpcnox net sarqes qces neteri mej nor kam verjum inch vor stuguma petq
			}
		}
		else {
			if (endPointsMap1.find(*first_i)->second.m_orient == Wire::orient::HORIZONTAL) {
				AV = new Wire(Wire::orient::VERTICAL, first_i->x);
				AV->m_startPoint = *first_i;
				if (gridIsFree(AV)) {
					m_firstWires.push_back(AV);
				}
				else {
					AV = new Wire(Wire::orient::VERTICAL, first_i->x - grid_offset);
					Point createdNewPoint(first_i->x - grid_offset, first_i->y);
					AV->m_startPoint = createdNewPoint;
					m_firstWires.push_back(AV);

					Wire* tempWire = new Wire(Wire::orient::HORIZONTAL, first_i->y);
					tempWire->m_endPoint = createdNewPoint;
					tempWire->m_startPoint = *first_i;
					endPointsMap1.insert(std::make_pair(createdNewPoint, *tempWire));
					m_lines[m_linesIndex].push_back(*tempWire);
					*first_i = createdNewPoint;

					// stex irakanum petqa 2 piner@ kpcnox net sarqes qces neteri mej nor kam verjum inch vor stuguma petq
				}
			}
			else {
				AH = new Wire(Wire::orient::HORIZONTAL, first_i->y);
				AH->m_startPoint = *first_i;
				if (gridIsFree(AH)) {
					m_firstWires.push_back(AH);
				}
				else {
					AH = new Wire(Wire::orient::HORIZONTAL, first_i->y - grid_offset);
					Point createdNewPoint(first_i->x, first_i->y - grid_offset);
					AH->m_startPoint = createdNewPoint;
					m_firstWires.push_back(AH);

					Wire* tempWire = new Wire(Wire::orient::VERTICAL, first_i->y);
					tempWire->m_endPoint = createdNewPoint;
					tempWire->m_startPoint = *first_i;
					endPointsMap1.insert(std::make_pair(createdNewPoint, *tempWire));
					m_lines[m_linesIndex].push_back(*tempWire);
					*first_i = createdNewPoint;

					// stex irakanum petqa 2 piner@ kpcnox net sarqes qces neteri mej nor kam verjum inch vor stuguma petq
				}
			}
		}
	}
}

void Router::createWiresOfSecondPin()
{
	Wire* AH = nullptr;
	Wire* AV = nullptr;
	m_secondWires.clear();
	std::vector<Point>::iterator second_i = m_secondPoints.begin();
	for (; second_i != m_secondPoints.end(); ++second_i) {
		if (endPointsMap2.empty()) {
			AH = new Wire(Wire::orient::HORIZONTAL, second_i->y);
			AH->m_startPoint = *second_i;
			AV = new Wire(Wire::orient::VERTICAL, second_i->x);
			AV->m_startPoint = *second_i;
			if (gridIsFree(AH)) {
				m_secondWires.push_back(AH);
			} else {
				AH = new Wire(Wire::orient::HORIZONTAL, second_i->y - grid_offset);
				Point createdNewPoint(second_i->x, second_i->y - grid_offset);
				AH->m_startPoint = createdNewPoint;
				m_secondWires.push_back(AH);

				Wire* tempWire = new Wire(Wire::orient::VERTICAL, second_i->x);
				tempWire->m_endPoint = createdNewPoint;
				tempWire->m_startPoint = *second_i;
				endPointsMap2.insert(std::make_pair(createdNewPoint, *tempWire));
				m_lines[m_linesIndex].push_back(*tempWire);
				*second_i = createdNewPoint;
			}
			if (gridIsFree(AV)) {
				m_secondWires.push_back(AV);
			}
			else {
				AV = new Wire(Wire::orient::VERTICAL, second_i->x - grid_offset);
				Point createdNewPoint(second_i->x - grid_offset, second_i->y);
				AV->m_startPoint = createdNewPoint;
				m_secondWires.push_back(AV);

				Wire* tempWire = new Wire(Wire::orient::HORIZONTAL, second_i->y);
				tempWire->m_endPoint = createdNewPoint;
				tempWire->m_startPoint = *second_i;
				endPointsMap2.insert(std::make_pair(createdNewPoint, *tempWire));
				m_lines[m_linesIndex].push_back(*tempWire);
				*second_i = createdNewPoint;
			}
		}
		else {
			if (endPointsMap2.find(*second_i)->second.m_orient == Wire::orient::HORIZONTAL) {
				AV = new Wire(Wire::orient::VERTICAL, second_i->x);
				AV->m_startPoint = *second_i;
				if (gridIsFree(AV)) {
					m_secondWires.push_back(AV);
				}
				else {
					AV = new Wire(Wire::orient::VERTICAL, second_i->x - grid_offset);
					Point createdNewPoint(second_i->x - grid_offset, second_i->y);
					AV->m_startPoint = createdNewPoint;
					m_secondWires.push_back(AV);

					Wire* tempWire = new Wire(Wire::orient::HORIZONTAL, second_i->y);
					tempWire->m_endPoint = createdNewPoint;
					tempWire->m_startPoint = *second_i;
					endPointsMap2.insert(std::make_pair(createdNewPoint, *tempWire));
					m_lines[m_linesIndex].push_back(*tempWire);
					*second_i = createdNewPoint;

					// stex irakanum petqa 2 piner@ kpcnox net sarqes qces neteri mej nor kam verjum inch vor stuguma petq
				}
			}
			else {
				AH = new Wire(Wire::orient::HORIZONTAL, second_i->y);
				AH->m_startPoint = *second_i;
				if (gridIsFree(AH)) {
					m_secondWires.push_back(AH);
				}
				else {
					AH = new Wire(Wire::orient::HORIZONTAL, second_i->y + grid_offset);
					Point createdNewPoint(second_i->x, second_i->y + grid_offset);
					AH->m_startPoint = createdNewPoint;
					m_secondWires.push_back(AH);

					Wire* tempWire = new Wire(Wire::orient::VERTICAL, second_i->y);
					tempWire->m_endPoint = createdNewPoint;
					tempWire->m_startPoint = *second_i;
					endPointsMap2.insert(std::make_pair(createdNewPoint, *tempWire));
					m_lines[m_linesIndex].push_back(*tempWire);
					*second_i = createdNewPoint;

					// stex irakanum petqa 2 piner@ kpcnox net sarqes qces neteri mej nor kam verjum inch vor stuguma petq
				}
			}
		}
	}

}

void Router::createSourcePoint(const libs::db_pin & source)
{
	m_source = source;
	int point_x = source.x;
	int point_y = source.y;
	if (source.x == source.parent->xl) {
		point_x = source.x - grid_offset;
	}
	else if (source.x == source.parent->xh) {
		point_x = source.x + grid_offset;
	}
	if (source.y == source.parent->yl) {
		point_y = source.y - grid_offset;
	}
	else if (source.y == source.parent->yh) {
		point_y = source.y + grid_offset;
	}
	Point* A = new Point(point_x, point_y);

	m_firstPoint = *A;
	m_firstPoints.push_back(*A);
}

void Router::createTargetPoint(const libs::db_pin & target)
{
	m_target = target;
	int point_x = target.x;
	int point_y = target.y;

	if (target.x == target.parent->xl) {
		point_x = target.x - grid_offset;
	}
	else if (target.x == target.parent->xh) {
		point_x = target.x + grid_offset;
	}
	if (target.y == target.parent->yl) {
		point_y = target.y - grid_offset;
	}
	else if (target.y == target.parent->yh) {
		point_y = target.y + grid_offset;
	}
	Point* B = new Point(point_x, point_y);
	m_secondPoint = *B;
	m_secondPoints.push_back(*B);

}

void Router::calculateRoutingArea()
{
	if (m_source.parent->xl < m_target.parent->xl) {
		m_x_min = m_source.parent->xl - grid_offset;
		m_x_max = m_target.parent->xh + grid_offset;
	}
	else {
		m_x_min = m_target.parent->xl - grid_offset;
		m_x_max = m_source.parent->xh + grid_offset;
	}
	if (m_source.parent->yl < m_target.parent->yl) {
		m_y_min = m_source.parent->yl - grid_offset;
		m_y_max = m_target.parent->yh + grid_offset;
	}
	else {
		m_y_min = m_target.parent->yl - grid_offset;
		m_y_max = m_source.parent->yh + grid_offset;
	}
}

Point Router::cultivateEndPointOfHorizontal(Point horizontalStart, Point verticalStart, Point* crossPoint, Wire* first)
{
	Point endPoint;
	if (first->m_startPoint.x < crossPoint->x) {
		Point* tempPointToCorner = new Point(m_x_min, first->m_startPoint.y);
		libs::db_cell* finded_cell = findNearestCellInRange(&first->m_startPoint, tempPointToCorner);
		if (finded_cell != nullptr) {
			if (first->m_orient == Wire::orient::HORIZONTAL) {
				int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
				endPoint = Point(new_x, crossPoint->y);
			}
			else {
				int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
				endPoint = Point(crossPoint->x, new_y);
			}
		}
		else {
			endPoint = Point(m_x_min, first->m_startPoint.y);
		}
	}
	else if (first->m_startPoint.x > crossPoint->x) {
		Point* tempPointToCorner = new Point(m_x_max, first->m_startPoint.y);
		libs::db_cell* finded_cell = findNearestCellInRange(&first->m_startPoint, tempPointToCorner);
		if (finded_cell != nullptr) {
			if (first->m_orient == Wire::orient::HORIZONTAL) {
				int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
				endPoint = Point(new_x, crossPoint->y);
			}
			else {
				int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
				endPoint = Point(crossPoint->x, new_y);
			}
		}
		else {
			endPoint = Point(m_x_max, first->m_startPoint.y);
		}
	}
	return endPoint;
}

Point Router::cultivateEndPointOfVertical(Point horizontalStart, Point verticalStart, Point* crossPoint, Wire* second)
{
	Point endPoint;
	if (second->m_startPoint.y < crossPoint->y) {
		Point* tempPointToCorner = new Point(second->m_startPoint.x, m_y_min);
		libs::db_cell* finded_cell = findNearestCellInRange(&second->m_startPoint, tempPointToCorner);
		if (finded_cell != nullptr) {
			if (second->m_orient == Wire::orient::HORIZONTAL) {
				int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
				endPoint = Point(new_x, crossPoint->y);
			}
			else {
				int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
				endPoint = Point(crossPoint->x, new_y);
			}
		}
		else {
			endPoint = Point(second->m_startPoint.x, m_y_min);
		}
	}
	else if (second->m_startPoint.y > crossPoint->y) {
		Point* tempPointToCorner = new Point(second->m_startPoint.x, m_y_max);
		libs::db_cell* finded_cell = findNearestCellInRange(&second->m_startPoint, tempPointToCorner);
		if (finded_cell != nullptr) {
			if (second->m_orient == Wire::orient::HORIZONTAL) {
				int new_x = (horizontalStart.x > finded_cell->xh) ? finded_cell->xh + grid_offset : finded_cell->xl - grid_offset;
				endPoint = Point(new_x, crossPoint->y);
			}
			else {
				int new_y = (verticalStart.y > finded_cell->yh) ? finded_cell->yh + grid_offset : finded_cell->yl - grid_offset;
				endPoint = Point(crossPoint->x, new_y);
			}
		}
		else {
			endPoint = Point(second->m_startPoint.x, m_y_max);
		}
	}
	return endPoint;
}

void Router::initRouter()
{
	m_linesIndex = 0;
	if (!m_firstPoints.empty()) {
		m_firstPoints.clear();
	}
	if (!m_secondPoints.empty()) {
		m_secondPoints.clear();
	}
	if (!m_firstWires.empty()) {
		m_firstWires.clear();
	}
	if (!m_secondWires.empty()) {
		m_secondWires.clear();
	}
	if (!endPointsMap1.empty()) {
		endPointsMap1.clear();
	}
	if (!endPointsMap2.empty()) {
		endPointsMap2.clear();
	}
	m_net = libs::Net();
}


// router that gets two cells
/*libs::Net Router::route(libs::db_cell first, libs::db_cell second)
{
	initRouter();
	m_first = first;
	m_second = second;

	//int netConditatesH = abs((second.yh - first.yl) / grid_offset);
	//int netConditatesV = abs((second.xh - first.xl) / grid_offset);

	// pini kordinatnerov
	libs::Net* net = new libs::Net();
	std::vector<Point*> crossingPoints;
	std::vector<Wire*> connection;

	// for first try as first point choose the haist right point
	//of first and lowest left point of second macro
	Point* A = new Point(first.xh, first.yh);
	Point* B = new Point(second.xl, second.yl);
	m_firstPoint = *A;
	m_secondPoint = *B;
	m_points.push_back(*A);
	m_points.push_back(*B);

	Wire* AH = nullptr;
	Wire* AV = nullptr;

	bool continueRout = true;
	while (continueRout) {
		std::vector<Point>::iterator i = m_points.begin();
		m_wires.clear();
		for (; i != m_points.end(); ++i) {
			AH = new Wire(Wire::orient::HORIZONTAL, i->y);
			AH->m_startPoint = *i;
			AV = new Wire(Wire::orient::VERTICAL, i->x);
			AV->m_startPoint = *i;
			m_wires.push_back(AH);
			m_wires.push_back(AV);
			m_lines[m_linesIndex].push_back(AH);
			m_lines[m_linesIndex].push_back(AV);
		}
		++m_linesIndex;
		m_lines.resize(m_linesIndex + 1);
		m_points.clear();
		std::vector<Wire*>::iterator j = m_wires.begin();
		std::vector<Wire*>::iterator k;
		Wire::orient orientation;
		bool continueCross = true;
		while((j != (m_wires.end() - 2)) && continueCross) {
			orientation = (*j)->m_orient;
			k = (orientation == Wire::orient::HORIZONTAL) ? j + 2 : j + 1;
			for (;  k != m_wires.end() && continueCross; ++k) {
				if ((orientation != (*k)->m_orient) && (crossed(*j, *k))) {
					continueCross = false;
				}
			}
			++j;
		}
		if (!continueCross) {
			continueRout = false;
		}
	}
	return	m_net;
}
*/
