#define __USE_MINGW_ANSI_STDIO 0

#include <regex>
#include <iostream>
#include <fstream>

#include "StringFunctions.h"
#include "Reserved.h"
#include "SyntaxValidator.h"
#include "AssemblyWriter.h"

// TODO: Functions + Recursion
// TODO: Loops + Nested Loops

/// Language Specs:
/// Case-Sensitive (All keywords are capital)
/// Strongly Typed (Can't change variable data type after first initialization)
/// Scoped 7 (Variables are only accessible within their definition-scope)
/// Variable Types: CHARACTER (UNSIGNED, 1 BYTE), INTEGER (SIGNED, 4 BYTE), std::string (UNSIGNED, 1 BYTE SEQUENCE), BOOLEAN (UNSIGNED, 1 BYTE)
/// std::string type variables can only be initialized at declaration stage, and values can't be changed after initialization
/// Variable names cannot start with __ (two underscores) or a digit, cannot have spaces, cannot repeat variable names
/// LET is used to declare/initialize a variable
/// After variable is declared using LET, SET is used for changing the value stored in it

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Wrong number of arguments provided. Usage: compiler <Source File> <Compiled Output>" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::ifstream inputFile(argv[1]); // Opens the file provided as first argument in Read Mode

	if (!inputFile.is_open())
	{
		std::cout << "ERROR: Cannot open input file. Exiting ..." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!fillReservedWords())
	{
		std::cout << "ERROR: Cannot open reserved words file. Exiting ..." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::stringstream dataSection;
	std::stringstream codeSection;

	std::string currLine;

	// Reading the file and adding all the std::strings to the variables map
	int printCounts = 0;
	int scopeNumber = 0;
	int lineNumber = 0;

	std::stack<std::string> scopeStack;
	scopeStack.push("GLOBAL");

	startProc("MAIN", codeSection);

	while (getline(inputFile, currLine))
	{
		lineNumber++;

		std::vector<std::string> tokens = tokenize(removeLeadingSpace(currLine));

		if (tokens.empty() || tokens[0].empty() || tokens[0] == "COMMENT")
		{
			// Ignoring empty/commented lines
		}

		else if (tokens[0] == "PRINT" || tokens[0] == "PRINTLINE")
		{
			if (!isValidPrint(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}
			
			if (tokens[1][0] == '"')
			{
				printCounts++;
				std::string varName = "__p" + std::to_string(printCounts);
				writePrintStatement(varName, escapeQuoteLiteral(tokens[1]), dataSection, codeSection, tokens[0] == "PRINTLINE");
			}

			else
			{
				writePrintStatement(tokens[1], getTypeFromEnum(variables[tokens[1]].type), codeSection, tokens[0] == "PRINTLINE");
			}
		}

		else if (tokens[0] == "LET")
		{
			if (!isValidLet(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			std::string varName = tokens[1];
			defineVariable(varName, Variable(getEnumFromType(tokens[3]), scopeStack.top()));

			if (tokens[3] == "STRING") writeLetStatement(varName, escapeQuoteLiteral(tokens[5]), dataSection);
			else writeLetStatement(varName, getASMDataSize(tokens[3]), tokens.size() == 6 ? tokens[5] : "0", dataSection);
		}

		else if (tokens[0] == "SET")
		{
			if (!isValidSet(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			std::string varName = tokens[1];
			std::string value = tokens[3];

			if (getVariableType(varName) != IntegerType && getVariableType(varName) != BooleanType) writeSetStatement(varName, value, codeSection);
			else
			{
				for (int i = 4; i < tokens.size(); i++)
				{
					value += " " + tokens[i];
				}

				std::deque<std::string> expr;
				if (getVariableType(varName) == IntegerType) expr = arithmetic_InfixToPostfix(value, lineNumber, scopeStack);
				else expr = logical_InfixToPostfix(value, lineNumber, scopeStack);

				if (expr.empty())
				{
					inputFile.close();
					exit(EXIT_FAILURE);
				}

				if (getVariableType(varName) == IntegerType) writeArithmeticExpression(expr, codeSection, varName);
				else
				{
					if (!isValidLogicalExpression(expr, lineNumber))
					{
						inputFile.close();
						exit(EXIT_FAILURE);
					}

					writeLogicalExpression(expr, codeSection, varName);
				}
			}
		}

		else if (tokens[0] == "IF")
		{
			if (!isValidIf(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			std::string value;

			for (int i = 1; i < tokens.size(); i++) value += " " + tokens[i];

			// Remove first space and bracket and last bracket
			value.erase(0, 2);
			value.erase(value.size() - 1, 1);

			std::deque<std::string> expr = logical_InfixToPostfix(value, lineNumber, scopeStack);

			if (expr.empty() || !isValidLogicalExpression(expr, lineNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			writeLogicalExpression(expr, codeSection, "");
			writeIfStatement(codeSection);

			scopeNumber++;
			scopeStack.push(std::to_string(scopeNumber) + "IF");
		}

		else if (tokens[0] == "ELSE")
		{
			writeElseStatement(codeSection);
		}

		else if (tokens[0] == "ENDIF")
		{
			if (!isValidEndIf(tokens, lineNumber, scopeStack.top(), scopeStack.size() - 1))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			writeEndIfStatement(codeSection);

			scopeStack.pop();
		}

		else if (tokens[0] == "INPUT")
		{
			if (!isValidInput(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			std::string varName = tokens[1];

			writeInputStatement(varName, codeSection);
		}

		else if (tokens[0] == "NEWLINE")
		{
			if (!isValidNewLine(tokens, lineNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			codeSection << "CALL CRLF\n";
		}

		else if (tokens[0] == "PAUSE")
		{
			if (!isValidPause(tokens, lineNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			codeSection << "CALL WaitMsg\n";
		}

		else
		{
			std::cout << "ERROR @ Line " << lineNumber << ": Statement is not a part of syntax." << std::endl;
			inputFile.close();
			exit(EXIT_FAILURE);
		}
	}

	endProc("MAIN", codeSection);

	inputFile.close();

	if (scopeStack.top() != "GLOBAL" || scopeStack.size() != 1)
	{
		std::cout << "Uncompilable source code. Invalid Scope. Please check whether all scopes are properly closed with \"END\" keyword." << std::endl;
		exit(EXIT_FAILURE);
	}

	writeAsmToFile((filenameWithoutExt(argv[1]) + ".asm").c_str(), dataSection, codeSection);

	system(("Assembler\\ml /c /nologo /Fo\"" + filenameWithoutExt(argv[1]) + ".obj\" /I \"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".asm").c_str());
	std::cout << " Linking: " + filenameWithoutExt(argv[1]) + ".obj irvine32.lib kernel32.lib user32.lib" << std::endl;
	system(("Linker\\link /OUT:\"" + filenameWithoutExt(argv[2]) + ".exe\" \"irvine32.lib\" \"kernel32.lib\" \"user32.lib\" /NOLOGO /SUBSYSTEM:CONSOLE /LIBPATH:\"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".obj").c_str());
	std::cout << " Saving: " + filenameWithoutExt(argv[2]) + ".exe" << std::endl;
	std::cout << " Deleting: " + filenameWithoutExt(argv[1]) + ".obj" << std::endl;
	system(("del " +  filenameWithoutExt(argv[1]) + ".obj").c_str());

	std::cout << "\n DONE!" << std::endl;

	// Runs the compiled executable
	//std::cout << "\n\n";
	//system("out.exe");

	return 0;
}
