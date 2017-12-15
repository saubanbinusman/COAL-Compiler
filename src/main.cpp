#define __USE_MINGW_ANSI_STDIO 0

#include <regex>
#include <iostream>
#include <fstream>

#include "StringFunctions.h"
#include "Reserved.h"
#include "SyntaxValidator.h"
#include "AssemblyWriter.h"

// TODO: Loops

int main(int argc, char** argv)
{
	std::cout  << "\n --- Pseudo Code Compiler ---\n";
	std::cout <<    "         Developed By        \n";
	std::cout <<    "       Sauban bin Usman      \n";
	std::cout <<    "          Ammar Adeel        \n";
	std::cout << std::endl;

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

	std::cout << " Reading   : " << argv[1] << std::endl;

	std::stringstream dataSection;
	std::stringstream codeSection;
	std::stringstream methodData;
	std::stringstream methodDataInitial;
	std::stringstream methodCode;

	std::string currLine;

	int printCounts = 0;
	int scopeNumber = 0;
	int lineNumber = 0;

	std::stack<std::string> scopeStack;

	scopeStack.push("GLOBAL");

	while (getline(inputFile, currLine))
	{
		lineNumber++;

		std::vector<std::string> tokens = tokenize(removeLeadingAndTrailingSpaces(currLine));

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
				writePrintStatement(varName, escapeQuoteLiteral(tokens[1]), dataSection, methodCode, tokens[0] == "PRINTLINE");
			}

			else
			{
				writePrintStatement(tokens[1], getTypeFromEnum(getVariableObject(tokens[1]).type), methodCode, tokens[0] == "PRINTLINE");
			}
		}

		else if (tokens[0] == "LET")
		{
			if (!isValidLet(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			const std::string& varName = tokens[1];
			defineVariable(varName, Variable(getEnumFromType(tokens[3]), scopeStack.top()));

			if (tokens[3] == "STRING")
			{
				writeLetStatementForString(varName, escapeQuoteLiteral(tokens[5]), dataSection);
			}

			else if (scopeStack.top() == "GLOBAL")
			{
				writeLetStatementForGlobal(varName, tokens.size() == 6 ? tokens[5] : std::string(), dataSection);
			}

			else
			{
				std::string val;
				if (tokens.size() == 6) val = tokens[5];
				else
				{
					if (tokens[3] == "INTEGER" || tokens[3] == "BOOLEAN") val = "0";
					else val = "'A'";
				}

				writeLetStatement(varName, getASMDataSize(tokens[3]), val, methodData, methodDataInitial);
			}
		}

		else if (tokens[0] == "SET")
		{
			if (!isValidSet(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			const std::string& varName = tokens[1];
			std::string value = tokens[3];

			if (tokens[3] == "CALL")
			{
				for (int i = 4; i < tokens.size(); i++)
				{
					value += " " + tokens[i];
				}

				std::vector<std::string> callTokens = regexTokenize(value, std::regex("[^\\s(,)]+"));
				writeCallStatement(callTokens[1], callTokens, methodCode, methodData);
				writeSetStatementForCall(varName, methodCode);
			}

			else
			{
				if (getVariableType(varName) == CharacterType) writeSetStatementForChar(varName, value, methodCode);
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

					if (getVariableType(varName) == IntegerType) writeArithmeticExpression(expr, methodCode, varName);
					else
					{
						if (!isValidLogicalExpression(expr, lineNumber))
						{
							inputFile.close();
							exit(EXIT_FAILURE);
						}

						writeLogicalExpression(expr, methodCode, varName);
					}
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

			writeLogicalExpression(expr, methodCode, "");
			writeIfStatement(methodCode);

			scopeNumber++;
			scopeStack.push(std::to_string(scopeNumber) + "IF");
		}

		else if (tokens[0] == "ELSE")
		{
			writeElseStatement(methodCode);
		}

		else if (tokens[0] == "ENDIF")
		{
			std::stack<std::string> temp = scopeStack;
			int closingNumber = -1;
			while (!temp.empty())
			{
				const std::string& top = temp.top();
				if (top[top.size() - 2] == 'I' && top[top.size() - 1] == 'F')
				{
					closingNumber = std::stoi(top);
					break;
				}

				temp.pop();
			}

			if (closingNumber == -1)
			{
				std::cout << "ERROR @ Line " << lineNumber << ": Improper IF/ENDIF pairs are causing an error." << std::endl;
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			if (!isValidEndIf(tokens, lineNumber, scopeStack.top(), closingNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			writeEndIfStatement(methodCode);

			scopeStack.pop();
		}

		else if (tokens[0] == "METHOD")
		{
			if (!isValidMethod(tokens, removeLeadingAndTrailingSpaces(currLine), lineNumber, scopeStack.top()))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			std::vector<std::string> methodTokens = regexTokenize(removeLeadingAndTrailingSpaces(currLine), std::regex("[^\\s,]+"));
			std::vector<Parameter> parametersList;

			for (int i = 3; i < methodTokens.size() - 2; i += 3)
			{
				Parameter p(methodTokens[i], getEnumFromType(methodTokens[i + 2]), false);
				if (i + 3 != methodTokens.size() - 2 && methodTokens[i + 3] == "REF")
				{
					p.reference = true;
					i++;
				}

				parametersList.push_back(p);
			}

			const std::string& methodName = methodTokens[1];
			const std::string& returnType = methodTokens[methodTokens.size() - 1];
			defineMethod(Method(methodName, parametersList, getEnumFromType(returnType), false));
			scopeStack.push(methodName);
			writeMethodStatement(methodName, parametersList, codeSection);
		}

		else if (tokens[0] == "ENDMETHOD")
		{
			if (!isValidEndMethod(tokens, lineNumber, scopeStack.top()))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			codeSection << methodData.str() << methodDataInitial.str() << "\n";
			codeSection << methodCode.str() << "\n";
			writeEndMethodStatement(scopeStack.top(), codeSection);
			codeSection << "\n";

			methodData.str(std::string());
			methodDataInitial.str(std::string());
			methodCode.str(std::string());
			methodDataInitial.clear();
			methodData.clear();
			methodCode.clear();

			scopeStack.pop();
		}

		else if (tokens[0] == "CALL")
		{
			if (!isValidCall(tokens, removeLeadingAndTrailingSpaces(currLine), lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			const std::vector<std::string>& callTokens = regexTokenize(removeLeadingAndTrailingSpaces(currLine), std::regex("[^\\s(,)]+"));
			writeCallStatement(callTokens[1], callTokens, methodCode, methodData);
		}

		else if (tokens[0] == "RETURN")
		{
			if (!isValidReturn(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			writeReturnStatement(getMethodInCurrentScope(scopeStack), tokens.size() == 2 ? tokens[1] : std::string(), methodCode);
			getMethodObject(getMethodInCurrentScope(scopeStack)).returned = true;
		}

		else if (tokens[0] == "INPUT")
		{
			if (!isValidInput(tokens, lineNumber, scopeStack))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			const std::string& varName = tokens[1];
			writeInputStatement(varName, methodCode);
		}

		else if (tokens[0] == "NEWLINE")
		{
			if (!isValidNewLine(tokens, lineNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			methodCode << "CALL CRLF\n";
		}

		else if (tokens[0] == "PAUSE")
		{
			if (!isValidPause(tokens, lineNumber, scopeStack.top()))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			methodCode << "CALL WaitMsg\n";
		}

		else
		{
			std::cout << "ERROR @ Line " << lineNumber << ": Statement is not a part of syntax." << std::endl;
			inputFile.close();
			exit(EXIT_FAILURE);
		}
	}

	inputFile.close();

	if (!isMethodDefined("MAIN"))
	{
		std::cout << "ERROR: Main method is not defined. Program cannot be compiled." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (scopeStack.top() != "GLOBAL" || scopeStack.size() != 1)
	{
		std::cout << "Uncompilable source code. Invalid Scope. Please check whether all scopes are properly closed with \"END\" keyword." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << " Writing   : " << (filenameWithoutExt(argv[1]) + ".asm") << std::endl;
	writeAsmToFile((filenameWithoutExt(argv[1]) + ".asm").c_str(), dataSection, codeSection);

	int success;

	success = system(("Assembler\\ml /c /nologo /Fo\"" + filenameWithoutExt(argv[1]) + ".obj\" /I \"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".asm").c_str());
	if (success != EXIT_SUCCESS)
	{
		exit(EXIT_FAILURE);
	}
	std::cout << " Assembled : " << (filenameWithoutExt(argv[1]) + ".obj") << std::endl;

	std::cout << " Linking   : " + filenameWithoutExt(argv[1]) + ".obj irvine32.lib kernel32.lib user32.lib" << std::endl;
	success = system(("Linker\\link /OUT:\"" + filenameWithoutExt(argv[2]) + ".exe\" \"irvine32.lib\" \"kernel32.lib\" \"user32.lib\" /NOLOGO /SUBSYSTEM:CONSOLE /LIBPATH:\"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".obj").c_str());
	if (success != EXIT_SUCCESS)
	{
		exit(EXIT_FAILURE);
	}
	std::cout << " Linked    : " << filenameWithoutExt(argv[2]) + ".exe" << std::endl;

	std::cout << " Deleting  : " + filenameWithoutExt(argv[1]) + ".obj" << std::endl;
	system(("del " +  filenameWithoutExt(argv[1]) + ".obj").c_str());

	return 0;
}
