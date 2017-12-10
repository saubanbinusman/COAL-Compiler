#include "VariablesMap.h"

std::map<std::string, Variable> variables;

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

bool isDefinedInAccessibleScopes(const std::string& variableName, std::stack<std::string> scopes)
{
	if (variables.find(variableName) == variables.end()) return false;

	while (!scopes.empty())
	{
		if (variables[variableName].scope == scopes.top()) return true;
		scopes.pop();
	}

	return false;
}

DataTypes getVariableType(const std::string& variableName)
{
	return variables[variableName].type;
}

void defineVariable(const std::string& variableName, const Variable& variable)
{
	variables[variableName] = variable;
}