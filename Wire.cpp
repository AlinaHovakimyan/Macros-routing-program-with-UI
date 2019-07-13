#include "Wire.h"
#include "Exception.h"

Wire::Wire(orient orientation, int coord) : m_orient(orientation) {
	switch (orientation) {
	case HORIZONTAL:
	{
		m_y = coord;
		break;
	}
	case VERTICAL:
	{
		m_x = coord;
		break;
	}
	default:
	{
		throw ExceptionError("Incorrect line orientation");
	}
	}
}