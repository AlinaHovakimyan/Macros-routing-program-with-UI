#ifndef DB_OBJ
#define DB_OBJ

enum type {
	CELL,
	PIN,
	NET,
	LINE,
	POINT,
	WIRE
};

class db_obj {
public:
	virtual type getType() = 0;
	// interface;
};

#endif
