#ifndef COAL_COMPILER_RESERVED_H
#define COAL_COMPILER_RESERVED_H

#include <set>
#include <fstream>
#include <iostream>

extern std::set<std::string> reservedWords;

bool fillReservedWords();
bool isReservedWord(const std::string&);

#endif //COAL_COMPILER_RESERVED_H
