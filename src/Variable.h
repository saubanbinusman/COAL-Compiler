#ifndef COAL_COMPILER_VARIABLE_H
#define COAL_COMPILER_VARIABLE_H

#include <string>

enum DataTypes
{
	IntegerType = 1,
	CharacterType = 2,
	StringType = 4,
	BooleanType = 8
};

struct Variable
{
	DataTypes type;
	std::string scope;

	Variable();
	Variable(DataTypes, const std::string&);
};

#endif //COAL_COMPILER_VARIABLE_H
