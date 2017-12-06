#ifndef COAL_COMPILER_STRINGFUNCTIONS_H
#define COAL_COMPILER_STRINGFUNCTIONS_H

#include <stack>
#include <deque>
#include <string>
#include <vector>
#include <iostream>

#include "VariablesMap.h"

std::string toUpperCase(const std::string&);
bool isOperator(const std::string&);
int getPrecedence(const std::string&);
bool isLeftAssociative(const std::string&);
bool isNumericValue(const std::string&);
std::string removeSpaces(const std::string&);
std::vector<std::string> expressionTokenizer(const std::string&);
std::deque<std::string> infixToPostfix(const std::string&, const int&);
std::vector<std::string> tokenize(const std::string&);
std::string escapeQuoteLiteral(const std::string&);
std::string filenameWithoutExt(char*);

#endif //COAL_COMPILER_STRINGFUNCTIONS_H
