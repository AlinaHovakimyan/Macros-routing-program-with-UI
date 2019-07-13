#ifndef ROUTE_
#define ROUTE_H

#include "db_cell.h"
#include "Net.h"
#include "Wire.h"
#include "Point.h"

#include <Map>
#include <list>

extern std::vector <std::vector<Wire>> m_lines;

class Router {
public:
	Router();
public:
	//libs::Net route(libs::db_cell first, libs::db_cell second);
	//libs::Net route(libs::db_pin first, libs::db_pin second);

	void route(std::vector<libs::Net> nets);
	libs::Net route(libs::db_pin first, libs::db_pin second);
	bool crossed(Wire* first, Wire* second);
	bool inPermissibleArea(Point* crossPoint, Wire* firstBegin, Wire* secondBegin);
	const std::vector<std::vector<Wire>>& getLines() const { return m_lines; };

	// finds nearest cell to point in the given range;
	libs::db_cell* findNearestCellInRange(Point* sourcePoint, Point* endPoint);

	//temp variables must be deleted from class
	//libs::db_cell m_first;
	//libs::db_cell m_second;
	
	libs::db_pin m_source;
	libs::db_pin m_target;
	
	Point m_firstPoint;
	Point m_secondPoint;
	std::vector<Point> m_firstPoints;
	std::vector<Point> m_secondPoints;
	std::list<Wire*> m_firstWires;
	std::list<Wire*> m_secondWires;
	//std::vector <std::vector<Wire*>> m_lines;
	int m_linesIndex;
	int m_x_min;
	int m_x_max;
	int m_y_min;
	int m_y_max;
	libs::Net m_net;

	std::multimap<Point, Wire> endPointsMap1;
	std::multimap<Point, Wire> endPointsMap2;
private:
	bool gridIsFree(const Wire* grid);
	void createWiresOfFirstPin();
	void createWiresOfSecondPin();
	void createSourcePoint(const libs::db_pin& source);
	void createTargetPoint(const libs::db_pin& target);
	void calculateRoutingArea();

	Point cultivateEndPointOfHorizontal(Point horizontalStart, Point verticalStart, Point* crossPoint, Wire* first);
	Point cultivateEndPointOfVertical(Point horizontalStart, Point verticalStart, Point* crossPoint, Wire* second);
private:
	void initRouter();
};

#endif