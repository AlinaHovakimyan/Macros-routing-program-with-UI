#ifndef TOKEN_INF_H
#define TOKEN_INF_H

#include <string>

enum tokenType {
	word,
	number,
	opening_bracket,
	closing_bracket,
	label,
	comma,
	semicolon,
};

struct TokenInf {
	union {
		std::string name;
		int value;
	};
	tokenType e_tokType;
	TokenInf(std::string name = "", tokenType type = tokenType()) :
		name(name), e_tokType(type)
	{}
	TokenInf(int value, tokenType type = tokenType()) :
		value(value), e_tokType(type)
	{}
	virtual ~TokenInf() {}
};

typedef TokenInf* TokenInfPtr;


#endif
