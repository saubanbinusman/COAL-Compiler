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
		codeSection << "XOR EAX, EAX\n";
		codeSection << "MOV AL, " << varName << "\n";
		codeSection << "CALL WriteDec\n";
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

void writeAsmToFile(const char* fileName, std::stringstream& dataSection, std::stringstream& codeSection)
{
	std::ofstream asmFile(fileName);

	if (!asmFile.is_open())
	{
		std::cout << "An error occurred while writing assembly file." << std::endl;
		exit(EXIT_FAILURE);
	}

	asmFile << "INCLUDE Irvine32.inc\n\n";
	asmFile << ".DATA\n";
	asmFile << dataSection.str();
	asmFile << "\n\n";
	asmFile << ".CODE\n";
	asmFile << codeSection.str();

	asmFile.flush();
	asmFile.close();
}

void writeExpression(std::deque<std::string> expr, std::stringstream& codeSection, const std::string& varName)
{
	std::stack<std::string> operands;
	
	codeSection << "PUSH EAX\n";
	codeSection << "PUSH EBX\n";
	codeSection << "PUSH ECX\n";
	codeSection << "PUSH EDX\n";
	
	while (!expr.empty())
	{
		std::cout << expr.front() << std::endl;
		
		if (isOperator(expr.front()))
		{
			if (expr.front() == "+")
			{
				writePOP("EBX", codeSection);
				writePOP("EAX", codeSection);
				
				writeADD("EAX", "EBX", codeSection);
				
				writePUSH("EAX", codeSection);
			}

			else if (expr.front() == "-")
			{
				writePOP("EBX", codeSection);
				writePOP("EAX", codeSection);
				
				writeSUB("EAX", "EBX", codeSection);
				
				writePUSH("EAX", codeSection);
			}

			else if (expr.front() == "*")
			{
				writePOP("EBX", codeSection);
				writePOP("EAX", codeSection);
				
				writeMUL("EAX", "EBX", codeSection);
				
				writePUSH("EAX", codeSection);
			}

			else if (expr.front() == "/")
			{
				writePOP("EBX", codeSection);
				writePOP("EAX", codeSection);
				
				writeDIV("EAX", "EBX", codeSection);
				
				writePUSH("EAX", codeSection);
			}

			else if (expr.front() == "%")
			{
				writePOP("EBX", codeSection);
				writePOP("EAX", codeSection);
				
				writeMOD("EAX", "EBX", codeSection);
				
				writePUSH("EAX", codeSection);
			}
			
			else if (expr.front() == "__p")
			{
				// Do nothing
				// writePOP("EAX", codeSection);
				// writePUSH("EAX", codeSection);
			}
			
			else if (expr.front() == "__m")
			{
				writePOP("EAX", codeSection);
				
				writeNEG("EAX", codeSection);
				
				writePUSH("EAX", codeSection);
			}

			else
			{
				writePOP("EBX", codeSection);
				writePOP("EAX", codeSection);
				
				writePOW("EAX", "EBX", codeSection);
				
				writePUSH("EAX", codeSection);
			}
		}

		else
		{
			writePUSH(expr.front(), codeSection);
		}
		
		expr.pop_front();
	}
	
	codeSection << "POP " << varName << "\n";
	
	codeSection << "POP EDX\n";
	codeSection << "POP ECX\n";
	codeSection << "POP EBX\n";
	codeSection << "POP EAX\n";
}

void writePUSH(const std::string& toPush, std::stringstream& codeSection)
{
	codeSection << "PUSH " << toPush << "\n";
}

void writePOP(const std::string& popTo, std::stringstream& codeSection)
{
	codeSection << "POP " << popTo << "\n";
}

void writeADD(const std::string& op1, const std::string& op2, std::stringstream& codeSection)
{
	codeSection << "MOV EAX, " << op1 << "\n";
	codeSection << "MOV EBX, " << op2 << "\n";
	codeSection << "ADD EAX, EBX\n";
}

void writeSUB(const std::string& op1, const std::string& op2, std::stringstream& codeSection)
{
	codeSection << "MOV EAX, " << op1 << "\n";
	codeSection << "MOV EBX, " << op2 << "\n";
	codeSection << "SUB EAX, EBX\n";
}

void writeMUL(const std::string& op1, const std::string& op2, std::stringstream& codeSection)
{
	codeSection << "MOV EAX, " << op1 << "\n";
	codeSection << "MOV EBX, " << op2 << "\n";
	codeSection << "IMUL EBX\n";
}

void writeDIV(const std::string& op1, const std::string& op2, std::stringstream& codeSection)
{
	codeSection << "XOR EDX, EDX\n";
	codeSection << "MOV EAX, " << op1 << "\n";
	codeSection << "MOV EBX, " << op2 << "\n";
	codeSection << "IDIV EBX\n";
}

void writeMOD(const std::string& op1, const std::string& op2, std::stringstream& codeSection)
{
	codeSection << "XOR EDX, EDX\n";
	codeSection << "MOV EAX, " << op1 << "\n";
	codeSection << "MOV EBX, " << op2 << "\n";
	codeSection << "IDIV EBX\n";
	codeSection << "MOV EAX, EDX\n";
}

void writeNEG(const std::string& op1, std::stringstream& codeSection)
{
	codeSection << "MOV EAX, " << op1 << "\n";
	codeSection << "NEG EAX\n";
}

void writePOW(const std::string& op1, const std::string& op2, std::stringstream& codeSection)
{
	codeSection << "MOV ECX, " << op2 << "\n";
	codeSection << "MOV EAX, " << op1 << "\n";
	codeSection << "MOV EBX, " << op2 << "\n";
	codeSection << "IMUL EBX\n";
}
