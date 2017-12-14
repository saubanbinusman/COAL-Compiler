#include "DefinitionsMap.h"

std::map<std::string, Variable> variables;
std::map<std::string, Method> methods;

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
		case UndefinedType: return "UNDEFINED";
	}
}

DataTypes getEnumFromType(const std::string& type)
{
	if (type == "INTEGER") return IntegerType;
	else if (type == "CHARACTER") return CharacterType;
	else if (type == "STRING") return StringType;
	else if (type == "BOOLEAN") return BooleanType;
	else return UndefinedType;
}

DataTypes getEnumFromValue(const std::string& value)
{
	if (value[0] == '\'') return CharacterType;
	if (isNumericValue(value)) return IntegerType;
	if (value[0] == '\"') return StringType;
	if (value == "TRUE" || value == "FALSE") return BooleanType;
	return UndefinedType;
}

bool isVariableDefinedInAccessibleScopes(const std::string& variableName, std::stack<std::string> scopes)
{
	if (variables.count(toUpperCase(variableName)) == 0) return false;

	const std::string& definedVarScope = variables.at(toUpperCase(variableName)).scope;

	while (!scopes.empty())
	{
		if (definedVarScope == scopes.top()) return true;
		scopes.pop();
	}

	return false;
}

bool isVariableDefinedInAllScopes(const std::string& variableName)
{
	return (variables.count(toUpperCase(variableName)) != 0);
}

DataTypes getVariableType(const std::string& variableName)
{
	return variables.at(toUpperCase(variableName)).type;
}

Variable& getVariableObject(const std::string& variableName)
{
	return variables.at(toUpperCase(variableName));
}

void defineVariable(const std::string& variableName, const Variable& variable)
{
	variables.insert(std::make_pair(toUpperCase(variableName), variable));
}

void defineMethod(const Method& method)
{
	methods.insert(std::make_pair(toUpperCase(method.name), method));
}

bool isMethodDefined(const std::string& methodName)
{
	return (methods.count(toUpperCase(methodName)) != 0);
}

DataTypes getMethodReturnType(const std::string& methodName)
{
	return methods.at(toUpperCase(methodName)).returnType;
}

std::vector<Parameter> getMethodParameters(const std::string& methodName)
{
	return methods.at(toUpperCase(methodName)).params;
}

bool isMethodReturned(const std::string& methodName)
{
	return methods.at(toUpperCase(methodName)).returned;
}

std::string getMethodInCurrentScope(std::stack<std::string> scope)
{
	while (!scope.empty())
	{
		if (isMethodDefined(scope.top())) return getMethodObject(scope.top()).name;
		scope.pop();
	}

	return std::string();
}

Method& getMethodObject(const std::string& methodName)
{
	return methods.at(toUpperCase(methodName));
}