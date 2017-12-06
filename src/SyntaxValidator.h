#ifndef COAL_COMPILER_SYNTAXVALIDATOR_H
#define COAL_COMPILER_SYNTAXVALIDATOR_H

#include <regex>
#include <vector>
#include <iostream>

bool isValidPrint(const std::vector<std::string>&, const int&);
bool isValidLet(const std::vector<std::string>&, const int&);
bool isValidSet(const std::vector<std::string>&, const int&);
bool isValidPause(const std::vector<std::string>&, const int&);
bool isValidNewLine(const std::vector<std::string>&, const int&);

#endif //COAL_COMPILER_SYNTAXVALIDATOR_H
