#ifndef COAL_COMPILER_ASSEMBLYWRITER_H
#define COAL_COMPILER_ASSEMBLYWRITER_H

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <deque>

void startProc(const std::string&, std::stringstream&);
void endProc(const std::string&, std::stringstream&);
void writePrintStatement(const std::string&, const std::string&, std::stringstream&, std::stringstream&, bool);
void writePrintStatement(const std::string&, const std::string&, std::stringstream&, bool);
void writeLetStatement(const std::string&, const std::string&, const std::string&, std::stringstream&);
void writeLetStatement(const std::string&, const std::string&, std::stringstream&);
void writeSetStatement(const std::string&, const std::string&, std::stringstream&);
void writeExpression(std::deque<std::string>, std::stringstream&, const std::string&);
void writePUSH(const std::string&, std::stringstream&);
void writePOP(const std::string&, std::stringstream&);
void writeADD(const std::string&, const std::string&, std::stringstream&);
void writeSUB(const std::string&, const std::string&, std::stringstream&);
void writeMUL(const std::string&, const std::string&, std::stringstream&);
void writeDIV(const std::string&, const std::string&, std::stringstream&);
void writeMOD(const std::string&, const std::string&, std::stringstream&);
void writeNEG(const std::string&, std::stringstream&);
void writePOW(const std::string&, const std::string&, std::stringstream&);
void writeAsmToFile(const char*, std::stringstream&, std::stringstream&);

#endif //COAL_COMPILER_ASSEMBLYWRITER_H
