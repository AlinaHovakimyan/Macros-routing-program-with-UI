#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

class ExceptionError {
	std::string exceptionMessage;
public:
	ExceptionError(std::string message) : exceptionMessage(message) {};
	std::string message() { return exceptionMessage; };
};

#endif