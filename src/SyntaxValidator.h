#ifndef COAL_COMPILER_SYNTAXVALIDATOR_H
#define COAL_COMPILER_SYNTAXVALIDATOR_H

#include <queue>
#include <stack>
#include <regex>
#include <vector>
#include <cctype>
#include <iostream>

bool isValidPrint(const std::vector<std::string>&, const int&, const std::stack<std::string>&);
bool isValidLet(const std::vector<std::string>&, const int&, const std::stack<std::string>&);
bool isValidSet(const std::vector<std::string>&, const int&, const std::stack<std::string>&);
bool isValidInput(const std::vector<std::string>&, const int&, const std::stack<std::string>&);
bool isValidIf(const std::vector<std::string>&, const int&, const std::stack<std::string>&);
bool isValidElseIf(const std::vector<std::string>&, const int&, const std::stack<std::string>&);
bool isValidEndIf(const std::vector<std::string>&, const int&, const std::string&, const int&);
bool isValidPause(const std::vector<std::string>&, const int&);
bool isValidNewLine(const std::vector<std::string>&, const int&);
bool isValidLogicalExpression(std::deque<std::string>, const int&);

#endif //COAL_COMPILER_SYNTAXVALIDATOR_H
