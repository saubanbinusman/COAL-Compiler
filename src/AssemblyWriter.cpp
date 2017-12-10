#include "AssemblyWriter.h"
#include "StringFunctions.h"

void startProc(const std::string& procName, std::stringstream& codeSection)
{
	codeSection << toUpperCase(procName) << " PROC\n";
}

void endProc(const std::string& procName, std::stringstream& codeSection)
{
	if (procName == "MAIN")
	{
		codeSection << "EXIT\n";
		codeSection << "MAIN ENDP\n";
		codeSection << "END MAIN\n";
	}

	else
	{
		codeSection << toUpperCase(procName) << " ENDP\n";
	}
}

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
		codeSection << "MOV EDX, OFFSET " << varName << "\n";
		codeSection << "CALL WriteString\n";
	}

	else if (type == "INTEGER")
	{
		codeSection << "PUSH EAX\n";
		codeSection << "MOV EAX, " << varName << "\n";
		codeSection << "CALL WriteInt\n";
		codeSection << "POP EAX\n";
	}

	else if (type == "CHARACTER")
	{
		codeSection << "PUSH EAX\n";
		codeSection << "MOV AL, " << varName << "\n";
		codeSection << "CALL WriteChar\n";
		codeSection << "POP EAX\n";
	}

	else
	{
		codeSection << "PUSH EAX\n";
		codeSection << "PUSH EDX\n";
		codeSection << "XOR EAX, EAX\n";
		codeSection << "MOV AL, " << varName << "\n";
		codeSection << "MOV BL, LENGTHOF FALSE_STR\n";
		codeSection << "MUL BL\n";
		codeSection << "MOV EDX, OFFSET FALSE_STR\n";
		codeSection << "ADD EDX, EAX\n";
		codeSection << "CALL WriteString\n";
		codeSection << "PUSH EDX\n";
		codeSection << "POP EAX\n";
	}

	if (newLine) codeSection << "CALL CRLF\n";
}

void writeLetStatement(const std::string& varName, const std::string& size, const std::string& initialValue, std::stringstream& dataSection)
{
	dataSection << varName << " " << size << " " << initialValue << "\n";
}

void writeLetStatement(const std::string& varName, const std::string& value, std::stringstream& dataSection)
{
	dataSection << varName << " BYTE " << value << ", 0\n";
}

void writeSetStatement(const std::string& varName, const std::string& value, std::stringstream& codeSection)
{
	codeSection << "MOV " << varName << ", " << value << "\n";
}

void writeArithmeticExpression(std::deque<std::string> expr, std::stringstream& codeSection, const std::string& varName)
{
	std::stack<std::string> operands;
	
	codeSection << "PUSH EAX\n";
	codeSection << "PUSH EBX\n";
	codeSection << "PUSH ECX\n";
	codeSection << "PUSH EDX\n";
	
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

		else
		{
			codeSection << "PUSH " << expr.front() << "\n";
		}
		
		expr.pop_front();
	}
	
	codeSection << "POP " << varName << "\n";
	
	codeSection << "POP EDX\n";
	codeSection << "POP ECX\n";
	codeSection << "POP EBX\n";
	codeSection << "POP EAX\n";
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
			if (expr.front() == "TRUE" || expr.front() == "FALSE" || getVariableType(expr.front()) == BooleanType)
			{
				codeSection << "MOV AL, " << expr.front() << "\n";
				codeSection << "PUSH EAX\n";
			}

			else if (isNumericValue(expr.front()) || getVariableType(expr.front()) == IntegerType)
			{
				codeSection << "MOV EAX, " << expr.front() << "\n";
				codeSection << "PUSH EAX\n";
			}
		}

		expr.pop_front();
	}

	codeSection << "POP EAX\n";

	if (!varName.empty())
	{
		codeSection << "MOV " << varName << ", AL\n";
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

void writeInputStatement(const std::string& varName, std::stringstream& codeSection)
{
	switch(getVariableType(varName))
	{
		case IntegerType:
			codeSection << "CALL ReadInt\n";
			codeSection << "MOV " << varName << ", EAX\n";
			break;

		case CharacterType:
			// TODO: To allow data to be shown when being input, use following when possible
			// codeSection << "MOV ECX, 1\n";
			// codeSection << "MOV EDX, OFFSET " << varName << "\n";
			// codeSection << "CALL ReadString\n";

			codeSection << "CALL ReadChar\n";
			codeSection << "MOV " << varName << ", AL\n";
			break;

		case BooleanType:
			codeSection << "CALL ReadInt\n";
			codeSection << "TEST EAX, EAX\n";
			codeSection << "LAHF\n"; // Load flags into AH (ZF = 6th bit (starting from 0th bit from right to left))
			codeSection << "SHR AH, 6\n";
			codeSection << "AND AH, 1\n";
			codeSection << "MOV " << varName << ", AH\n";
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
	asmFile << "TRUE EQU 1\n";
	asmFile << ".DATA\n";
	asmFile << "FALSE_STR BYTE \"FALSE\", 0\n";
	asmFile << "TRUE_STR BYTE \"TRUE\", 0\n";
	asmFile << dataSection.str();
	asmFile << "\n\n";
	asmFile << ".CODE\n";
	asmFile << codeSection.str();

	asmFile.flush();
	asmFile.close();
}