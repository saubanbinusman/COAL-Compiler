#ifndef COAL_COMPILER_STRINGFUNCTIONS_H
#define COAL_COMPILER_STRINGFUNCTIONS_H

#include <stack>
#include <deque>
#include <regex>
#include <string>
#include <vector>
#include <iostream>

#include "DefinitionsMap.h"

std::string toUpperCase(const std::string&);
std::string removeLeadingSpace(const std::string&);
bool arithmetic_IsOperator(const std::string&);
int arithmetic_GetPrecedence(const std::string&);
bool arithmetic_IsLeftAssociative(const std::string&);
std::vector<std::string> arithmetic_ExpressionTokenizer(const std::string&);
std::deque<std::string> arithmetic_InfixToPostfix(const std::string&, const int&, const std::stack<std::string>&);
bool logical_IsOperator(const std::string&);
bool logical_IsComparator(const std::string&);
int logical_GetPrecedence(const std::string&);
bool logical_IsLeftAssociative(const std::string&);
std::vector<std::string> logical_ExpressionTokenizer(const std::string&);
std::deque<std::string> logical_InfixToPostfix(const std::string&, const int&, const std::stack<std::string>&);
bool isNumericValue(const std::string&);
std::string removeSpaces(const std::string&);
std::string removeBrackets(const std::string&);
std::string removeSpacesAndBrackets(const std::string&);
std::vector<std::string> tokenize(const std::string&);
std::vector<std::string> regexTokenize(const std::string&, const std::regex&);
std::string escapeQuoteLiteral(const std::string&);
std::string filenameWithoutExt(char*);

#endif //COAL_COMPILER_STRINGFUNCTIONS_H
