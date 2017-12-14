#ifndef COAL_COMPILER_PARAMETER_H
#define COAL_COMPILER_PARAMETER_H

#include <string>
#include "Variable.h"

struct Parameter
{
	std::string name;
	DataTypes type;
	bool reference;

	Parameter(const std::string&, const DataTypes&, const bool&);
};

#endif //COAL_COMPILER_PARAMETER_H
