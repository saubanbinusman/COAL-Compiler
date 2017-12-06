#include "VariablesMap.h"

std::map<std::string, DataTypes> variables;

std::string getASMDataSize(const std::string& type)
{
	if (type == "CHARACTER" || type == "STRING" || type == "BOOLEAN") return "BYTE";
	else return "SDWORD";
}

std::string getTypeFromEnum(DataTypes type)
{
	switch(type)
	{
		case IntegerType: return "INTEGER";
		case CharacterType: return "CHARACTER";
		case StringType: return "STRING";
		case BooleanType: return "BOOLEAN";
	}
}

DataTypes getEnumFromType(const std::string& type)
{
	if (type == "INTEGER") return IntegerType;
	else if (type == "CHARACTER") return CharacterType;
	else if (type == "STRING") return StringType;
	else return BooleanType;
}

bool isVariableDefined(const std::string& variableName)
{
	return (variables.find(variableName) != variables.end());
}

DataTypes getVariableType(const std::string& variableName)
{
	return variables[variableName];
}

void defineVariable(const std::string& variableName, DataTypes type)
{
	variables[variableName] = type;
}