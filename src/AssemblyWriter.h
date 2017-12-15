#ifndef COAL_COMPILER_ASSEMBLYWRITER_H
#define COAL_COMPILER_ASSEMBLYWRITER_H

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <deque>
#include "StringFunctions.h"
#include "Parameter.h"

void writePrintStatement(const std::string&, const std::string&, std::stringstream&, std::stringstream&, bool);
void writePrintStatement(const std::string&, const std::string&, std::stringstream&, bool);
void writeLetStatement(const std::string&, const std::string&, const std::string&, std::stringstream&, std::stringstream&);
void writeLetStatementForString(const std::string&, const std::string&, std::stringstream&);
void writeLetStatementForGlobal(const std::string&, const std::string&, std::stringstream&);
void writeSetStatementForChar(const std::string&, const std::string&, std::stringstream&);
void writeSetStatementForCall(const std::string&, std::stringstream&);
void writeArithmeticExpression(std::deque<std::string>, std::stringstream&, const std::string&);
void writeLogicalExpression(std::deque<std::string>, std::stringstream&, const std::string&);
void writeIfStatement(std::stringstream&);
void writeElseStatement(std::stringstream&);
void writeEndIfStatement(std::stringstream&);
void writeMethodStatement(const std::string&, const std::vector<Parameter>&, std::stringstream&);
void writeEndMethodStatement(const std::string&, std::stringstream&);
void writeReturnStatement(const std::string&, const std::string&, std::stringstream&);
void writeCallStatement(const std::string&, const std::vector<std::string>&, std::stringstream&, std::stringstream&);
void writeInputStatement(const std::string&, std::stringstream&);
void writeAsmToFile(const char*, std::stringstream&, std::stringstream&);

#endif //COAL_COMPILER_ASSEMBLYWRITER_H
