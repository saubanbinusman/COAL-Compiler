#include "AssemblyWriter.h"

void writePrintStatement(const std::string& varName, const std::string& toPrint, std::stringstream& dataSection, std::stringstream& codeSection, bool newLine)
{
	dataSection << varName << " BYTE " << toPrint << ", 0\n";

	codeSection << "MOV EDX, OFFSET " << varName << "\n";
	codeSection << "CALL WriteString\n";
	if (newLine) codeSection << "CALL CRLF\n";
}

void writePrintStatement(const std::string& varName, const std::string& type, std::stringstream& codeSection, bool newLine)
{
	if (type == "STRING")
	{
		if (getVariableObject(varName).reference) codeSection << "MOV EDX, " << varName << "\n";
		else codeSection << "MOV EDX, OFFSET " << varName << "\n";
		codeSection << "CALL WriteString\n";
	}

	else if (type == "INTEGER")
	{
		if (getVariableObject(varName).reference)
		{
			codeSection << "MOV EBX, " << varName << "\n";
			codeSection << "MOV EAX, [EBX]\n";
		}

		else codeSection << "MOV EAX, " << varName << "\n";

		codeSection << "CALL WriteInt\n";
	}

	else if (type == "CHARACTER")
	{
		if (getVariableObject(varName).reference)
		{
			codeSection << "MOV EBX, " << varName << "\n";
			codeSection << "MOV AL, [EBX]\n";
		}

		else codeSection << "MOV AL, " << varName << "\n";

		codeSection << "CALL WriteChar\n";
	}

	else
	{
		codeSection << "XOR EAX, EAX\n";

		if (getVariableObject(varName).reference)
		{
			codeSection << "MOV EBX, " << varName << "\n";
			codeSection << "MOV AL, [EBX]\n";
		}

		else codeSection << "MOV AL, " << varName << "\n";

		codeSection << "MOV BL, LENGTHOF FALSE_STR\n";
		codeSection << "MUL BL\n";
		codeSection << "MOV EDX, OFFSET FALSE_STR\n";
		codeSection << "ADD EDX, EAX\n";
		codeSection << "CALL WriteString\n";
	}

	if (newLine) codeSection << "CALL CRLF\n";
}

void writeLetStatement(const std::string& varName, const std::string& size, const std::string& initialValue, std::stringstream& dataSection, std::stringstream& dataInitial)
{
	dataSection << "LOCAL " << varName << ":" << size << "\n";
	dataInitial << "MOV " << varName << ", " << initialValue << "\n";
}

void writeLetStatementForString(const std::string& varName, const std::string& value, std::stringstream& dataSection)
{
	dataSection << varName << " BYTE " << value << ", 0\n";
}

void writeLetStatementForGlobal(const std::string& varName, const std::string& value, std::stringstream& dataSection)
{
	dataSection << varName << " " << getASMDataSize(getTypeFromEnum(getVariableType(varName))) << " ";

	if (value.empty())
	{
		switch (getVariableType(varName))
		{
			case BooleanType:
				dataSection << "FALSE";
				break;

			case CharacterType:
				dataSection << "'A'";
				break;

			case IntegerType:
				dataSection << "0";
				break;
		}
	}

	else
	{
		dataSection << value;
	}

	dataSection << "\n";
}

void writeSetStatementForChar(const std::string& varName, const std::string& value, std::stringstream& codeSection)
{
	if (getVariableObject(varName).reference)
	{
		codeSection << "MOV EBX, " << varName << "\n";
		codeSection << "MOV AL, " << value << "\n";
		codeSection << "MOV [EBX], AL\n";
	}

	else
	{
		codeSection << "MOV AL, " << value << "\n";
		codeSection << "MOV " << varName << ", AL\n";
	}
}

// Relies on ESI value from writeCallStatement() method
void writeSetStatementForCall(const std::string& varName, std::stringstream& codeSection)
{
	codeSection << "MOV EAX, ESI\n";

	if (getVariableObject(varName).reference)
	{
		codeSection << "MOV EBX, " << varName << "\n";

		if (getVariableType(varName) == IntegerType) codeSection << "MOV [EBX], EAX\n";
		else codeSection << "MOV [EBX], AL\n";
	}

	else
	{
		if (getVariableType(varName) == IntegerType) codeSection << "MOV " << varName << ", EAX\n";
		else codeSection << "MOV " << varName << ", AL\n";
	}
}

void writeArithmeticExpression(std::deque<std::string> expr, std::stringstream& codeSection, const std::string& varName)
{
	std::stack<std::string> operands;
	
	while (!expr.empty())
	{
		if (arithmetic_IsOperator(expr.front()))
		{
			if (expr.front() == "+")
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "ADD EAX, EBX\n";
				codeSection << "PUSH EAX\n";
			}

			else if (expr.front() == "-")
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "SUB EAX, EBX\n";
				codeSection << "PUSH EAX\n";
			}

			else if (expr.front() == "*")
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "IMUL EBX\n";
				codeSection << "PUSH EAX\n";
			}

			else if (expr.front() == "/")
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "XOR EDX, EDX\n";
				codeSection << "IDIV EBX\n";
				codeSection << "PUSH EAX\n";
			}

			else if (expr.front() == "%")
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "XOR EDX, EDX\n";
				codeSection << "IDIV EBX\n";
				codeSection << "PUSH EDX\n";
			}
			
			else if (expr.front() == "__p")
			{
				// Do nothing
			}
			
			else if (expr.front() == "__m")
			{
				codeSection << "POP EAX\n";
				codeSection << "NEG EAX\n";
				codeSection << "PUSH EAX\n";
			}

			// Naive Exponentiation ASM Code
			else
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "MOV ECX, EBX\n";
				codeSection << "DEC ECX\n";
				codeSection << "MOV EBX, EAX\n";
				codeSection << ".WHILE (ECX)\n";
				codeSection << "MUL EBX\n";
				codeSection << "DEC ECX\n";
				codeSection << ".ENDW\n";
				codeSection << "PUSH EAX\n";
			}
		}

		// Is a value or variable
		else
		{
			if (isVariableDefinedInAllScopes(expr.front()))
			{
				if (getVariableObject(expr.front()).reference)
				{
					codeSection << "MOV EBX, " << expr.front() << "\n";
					codeSection << "MOV EAX, [EBX]\n";
					codeSection << "PUSH EAX\n";
				}

				else codeSection << "PUSH " << expr.front() << "\n";
			}

			else codeSection << "PUSH " << expr.front() << "\n";
		}
		
		expr.pop_front();
	}

	if (getVariableObject(varName).reference)
	{
		codeSection << "MOV EBX, " << varName << "\n";
		codeSection << "POP EAX\n";
		codeSection << "MOV [EBX], EAX\n";
	}

	else
	{
		codeSection << "POP " << varName << "\n";
	}
}

void writeLogicalExpression(std::deque<std::string> expr, std::stringstream& codeSection, const std::string& varName)
{
	std::stack<std::string> operands;

	codeSection << "XOR EAX, EAX\n";

	while (!expr.empty())
	{
		if (logical_IsOperator(expr.front()))
		{
			if (expr.front() == "NOT")
			{
				codeSection << "POP EAX\n";
				codeSection << "XOR AL, 1\n";
				codeSection << "PUSH EAX\n";
			}

			else if (expr.front() == "AND")
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "AND AL, BL\n";
				codeSection << "PUSH EAX\n";
			}

			else if (expr.front() == "OR")
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << "OR AL, BL\n";
				codeSection << "PUSH EAX\n";
			}

			else if (logical_IsComparator(expr.front()))
			{
				codeSection << "POP EBX\n";
				codeSection << "POP EAX\n";
				codeSection << ".IF (EAX " << expr.front() << " EBX)\n";
				codeSection << "PUSH TRUE\n";
				codeSection << ".ELSE\n";
				codeSection << "PUSH FALSE\n";
				codeSection << ".ENDIF\n";
			}
		}

		else
		{
			if (expr.front() == "TRUE" || expr.front() == "FALSE")
			{
				codeSection << "MOV AL, " << expr.front() << "\n";
			}

			else if (isNumericValue(expr.front()))
			{
				codeSection << "MOV EAX, " << expr.front() << "\n";
			}

			else if (getVariableType(expr.front()) == BooleanType)
			{
				if (getVariableObject(expr.front()).reference)
				{
					codeSection << "MOV EBX, " << expr.front() << "\n";
					codeSection << "MOV AL, [EBX]\n";
				}

				else
				{
					codeSection << "MOV AL, " << expr.front() << "\n";
				}
			}

			else if (getVariableType(expr.front()) == IntegerType)
			{
				if (getVariableObject(expr.front()).reference)
				{
					codeSection << "MOV EBX, " << expr.front() << "\n";
					codeSection << "MOV EAX, [EBX]\n";
				}

				else
				{
					codeSection << "MOV EAX, " << expr.front() << "\n";
				}
			}

			codeSection << "PUSH EAX\n";
		}

		expr.pop_front();
	}

	codeSection << "POP EAX\n";

	if (!varName.empty())
	{
		if (getVariableObject(varName).reference)
		{
			codeSection << "MOV EBX, " << varName << "\n";
			codeSection << "MOV [EBX], AL\n";
		}

		else codeSection << "MOV " << varName << ", AL\n";
	}
}

// Relies on EAX value from writeLogicalExpression() method
void writeIfStatement(std::stringstream& codeSection)
{
	codeSection << ".IF (EAX)\n";
}

void writeElseStatement(std::stringstream& codeSection)
{
	codeSection << ".ELSE\n";
}

void writeEndIfStatement(std::stringstream& codeSection)
{
	codeSection << ".ENDIF\n";
}

void writeMethodStatement(const std::string& methodName, const std::vector<Parameter>& paramList, std::stringstream& codeSection)
{
	codeSection << methodName << " PROC";

	if (paramList.size() != 0) codeSection << " ";

	for (int i = 0; i < paramList.size(); i++)
	{
		defineVariable(paramList[i].name, Variable(paramList[i].type, methodName, paramList[i].reference || paramList[i].type == StringType));
		codeSection << paramList[i].name << ":";
		if (paramList[i].reference || paramList[i].type == StringType) codeSection << "PTR ";
		codeSection << getASMDataSize(getTypeFromEnum(paramList[i].type));
		if (i != paramList.size() - 1) codeSection << ", ";
	}

	codeSection << "\n";
}

void writeEndMethodStatement(const std::string& methodName, std::stringstream& codeSection)
{
	if (methodName == "MAIN")
	{
		codeSection << "EXIT\n";
		codeSection << "MAIN ENDP\n";
		codeSection << "END MAIN\n";
	}

	else
	{
		switch (getMethodReturnType(methodName))
		{
			case BooleanType:
				codeSection << "MOV ESI, FALSE\n";
				break;

			case CharacterType:
				codeSection << "MOV ESI, 'A'\n";
				break;

			case IntegerType:
				codeSection << "MOV ESI, 0\n";
				break;
		}

		codeSection << "RET\n";
		codeSection << toUpperCase(methodName) << " ENDP\n";
	}
}

void writeReturnStatement(const std::string& methodName, const std::string& value, std::stringstream& codeSection)
{
	if (value.empty())
	{
		codeSection << "RET\n";
	}

	else
	{
		switch (getMethodReturnType(methodName))
		{
			case BooleanType:
				codeSection << "XOR EDX, EDX\n";
				codeSection << "MOV DL, " << value << "\n";
				codeSection << "MOV ESI, EDX\n";
				break;

			case CharacterType:
				codeSection << "XOR EDX, EDX\n";
				codeSection << "MOV DL, " << value << "\n";
				codeSection << "MOV ESI, EDX\n";
				break;

			case IntegerType:
				codeSection << "MOV ESI, " << value << "\n";
				break;
		}

		codeSection << "RET\n";
	}
}

void writeCallStatement(const std::string& methodName, const std::vector<std::string>& callTokens, std::stringstream& codeSection, std::stringstream& methodData)
{
	std::stringstream code;

	code << "INVOKE " << methodName;

	if (callTokens.size() > 2)
	{
		code << ", ";

		const std::vector<Parameter>& params = getMethodParameters(methodName);
		int specialCaseCount = 0;

		for (int i = 2; i < callTokens.size(); i++)
		{
			// No need to check for specific scope because it is already checked in isValidCall() function
			if (isVariableDefinedInAllScopes(callTokens[i]))
			{
				if (getVariableObject(callTokens[i]).reference && params[i - 2].reference)
				{
					code << callTokens[i];
				}

				else if (getVariableObject(callTokens[i]).reference && !params[i - 2].reference)
				{
					if (getVariableType(callTokens[i]) == StringType)
					{
						code << callTokens[i];
					}

					else
					{
						specialCaseCount++;
						methodData << "LOCAL " << ("__specialCaseVariable" + std::to_string(specialCaseCount)) << ":" << getASMDataSize(getTypeFromEnum(getVariableObject(callTokens[i]).type)) << "\n";
						codeSection << "MOV EBX, " << callTokens[i] << "\n";
						codeSection << "XOR EAX, EAX\n";

						switch (getVariableType(callTokens[i]))
						{
							case BooleanType:
								codeSection << "MOV AL, [EBX]\n";
								codeSection << "MOV __specialCaseVariable" << specialCaseCount << ", AL\n";
								break;

							case CharacterType:
								codeSection << "MOV AL, [EBX]\n";
								codeSection << "MOV __specialCaseVariable" << specialCaseCount << ", AL\n";
								break;

							case IntegerType:
								codeSection << "MOV EAX, [EBX]\n";
								codeSection << "MOV __specialCaseVariable" << specialCaseCount << ", EAX\n";
								break;
						}
					}
				}

				else if (!getVariableObject(callTokens[i]).reference && params[i - 2].reference)
				{
					code << "ADDR " << callTokens[i];
				}

				else
				{
					if (getVariableType(callTokens[i]) == StringType) code << "ADDR " << callTokens[i];
					else code << callTokens[i];
				}
			}

			else
			{
				code << callTokens[i];
			}

			if (i != callTokens.size() - 1) code << ", ";
		}
	}

	code << "\n";
	codeSection << code.str();
}

void writeInputStatement(const std::string& varName, std::stringstream& codeSection)
{
	switch (getVariableType(varName))
	{
		case IntegerType:
			codeSection << "CALL ReadInt\n";

			if (getVariableObject(varName).reference)
			{
				codeSection << "MOV EBX, " << varName << "\n";
				codeSection << "MOV [EBX], EAX\n";
			}

			else codeSection << "MOV " << varName << ", EAX\n";
			break;

		case CharacterType:
			codeSection << "CALL ReadChar\n";

			if (getVariableObject(varName).reference)
			{
				codeSection << "MOV EBX, " << varName << "\n";
				codeSection << "MOV [EBX], AL\n";
			}

			else codeSection << "MOV " << varName << ", AL\n";
			break;

		case BooleanType:
			codeSection << "CALL ReadInt\n";
			codeSection << "TEST EAX, EAX\n";
			codeSection << "LAHF\n"; // Load flags into AH (ZF = 6th bit (starting from 0th bit from right to left))
			codeSection << "SHR AH, 6\n";
			codeSection << "AND AH, 1\n";

			if (getVariableObject(varName).reference)
			{
				codeSection << "MOV EBX, " << varName << "\n";
				codeSection << "MOV [EBX], AH\n";
			}

			else codeSection << "MOV " << varName << ", AH\n";
			break;
	}
}

void writeAsmToFile(const char* fileName, std::stringstream& dataSection, std::stringstream& codeSection)
{
	std::ofstream asmFile(fileName);

	if (!asmFile.is_open())
	{
		std::cout << "An error occurred while writing assembly file." << std::endl;
		exit(EXIT_FAILURE);
	}

	asmFile << "INCLUDE Irvine32.inc\n\n";
	asmFile << ".CONST\n";
	asmFile << "FALSE EQU 0\n";
	asmFile << "TRUE EQU 1\n\n";
	asmFile << ".DATA\n";
	asmFile << "FALSE_STR BYTE \"FALSE\", 0\n";
	asmFile << "TRUE_STR BYTE \"TRUE\", 0\n";
	asmFile << dataSection.str();
	asmFile << "\n";
	asmFile << ".CODE\n";
	asmFile << codeSection.str();

	asmFile.flush();
	asmFile.close();
}
