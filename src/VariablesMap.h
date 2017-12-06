#ifndef COAL_COMPILER_VARIABLESMAP_H
#define COAL_COMPILER_VARIABLESMAP_H

#include <map>

enum DataTypes
{
	IntegerType = 1,
	CharacterType = 2,
	StringType = 4,
	BooleanType = 8
};

extern std::map<std::string, DataTypes> variables;

std::string getASMDataSize(const std::string&);
std::string getTypeFromEnum(DataTypes);
DataTypes getEnumFromType(const std::string&);
bool isVariableDefined(const std::string&);
DataTypes getVariableType(const std::string&);
void defineVariable(const std::string&, DataTypes);

#endif //COAL_COMPILER_VARIABLESMAP_H
