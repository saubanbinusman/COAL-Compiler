#ifndef COAL_COMPILER_DEFINITIONSMAP_H
#define COAL_COMPILER_DEFINITIONSMAP_H

#include <map>
#include <stack>
#include "Method.h"
#include "Variable.h"
#include "Parameter.h"
#include "StringFunctions.h"

extern std::map<std::string, Variable> variables;
extern std::map<std::string, Method> methods;

std::string getASMDataSize(const std::string&);
std::string getTypeFromEnum(DataTypes);
DataTypes getEnumFromType(const std::string&);
DataTypes getEnumFromValue(const std::string&);
bool isVariableDefinedInAccessibleScopes(const std::string&, std::stack<std::string>);
bool isVariableDefinedInAllScopes(const std::string&);
DataTypes getVariableType(const std::string&);
Variable& getVariableObject(const std::string&);
void defineVariable(const std::string&, const Variable&);
void defineMethod(const Method&);
bool isMethodDefined(const std::string&);
DataTypes getMethodReturnType(const std::string&);
std::vector<Parameter> getMethodParameters(const std::string&);
bool isMethodReturned(const std::string&);
std::string getMethodInCurrentScope(std::stack<std::string>);
Method& getMethodObject(const std::string&);

#endif //COAL_COMPILER_DEFINITIONSMAP_H
