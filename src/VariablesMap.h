#ifndef COAL_COMPILER_VARIABLESMAP_H
#define COAL_COMPILER_VARIABLESMAP_H

#include <map>
#include <stack>
#include "Variable.h"

extern std::map<std::string, Variable> variables;

std::string getASMDataSize(const std::string&);
std::string getTypeFromEnum(DataTypes);
DataTypes getEnumFromType(const std::string&);
bool isDefinedInAccessibleScopes(const std::string&, std::stack<std::string>);
DataTypes getVariableType(const std::string&);
void defineVariable(const std::string&, const Variable&);

#endif //COAL_COMPILER_VARIABLESMAP_H
