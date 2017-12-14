#ifndef COAL_COMPILER_VARIABLE_H
#define COAL_COMPILER_VARIABLE_H

#include <string>

enum DataTypes
{
	UndefinedType = 0,
	IntegerType = 1,
	CharacterType = 2,
	StringType = 4,
	BooleanType = 8
};

struct Variable
{
	DataTypes type;
	std::string scope;
	bool reference;

	Variable(DataTypes, const std::string&);
	Variable(DataTypes, const std::string&, const bool&);
};

#endif //COAL_COMPILER_VARIABLE_H
