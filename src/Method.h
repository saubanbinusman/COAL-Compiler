#ifndef COAL_COMPILER_METHOD_H
#define COAL_COMPILER_METHOD_H

#include <vector>
#include "Parameter.h"

struct Method
{
	std::string name;
	std::vector<Parameter> params;
	DataTypes returnType;
	bool returned;

	Method(const std::string&, const std::vector<Parameter>&, const DataTypes&, const bool&);
};

#endif //COAL_COMPILER_METHOD_H
