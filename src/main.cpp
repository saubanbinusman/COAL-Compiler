#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

typedef enum DataTypes
{
	Integer = 1,
	Character = 2,
	String = 4
} DataType;

map < pair<string, DataType>, void* > variables;

string firstWord(const string& line)
{
	return line.substr(0, line.find_first_of(' '));
}

string filenameWithoutExt(char* fileName)
{
	string withoutExt(fileName);
	return withoutExt.substr(0, withoutExt.find_last_of('.'));
}

// Returns false if any error occurs during the process
bool writeIncludesAndDataSection(stringstream& asmCode)
{
	asmCode << "INCLUDE Irvine32.inc\n\n";
	asmCode << ".DATA\n";

	for (auto it : variables)
	{
		string type;
		bool strVal = false;

		switch(it.first.second)
		{
			case Integer:
				type = "DWORD";
				break;

			case Character:
				type = "BYTE";
				break;

			case String:
				type = "BYTE";
				strVal = true;
				break;

			default:
				// Unknown type for a variable detected
				return false;
		}

		if (strVal)
		{
			string data(*(string*)(it.second));
			asmCode << (it.first.first) << " " << type << " " << data << ", 0\n";
		}

		else
		{
			int data = *(int*)(it.second);
			asmCode << (it.first.first) << " " << type << " " << data << "\n";
		}
	}

	return true;
}

void startCodeSection(stringstream& asmCode)
{
	asmCode << "\n";
	asmCode << ".CODE\n";
	asmCode << "MAIN PROC\n";
}

void endCodeSection(stringstream& asmCode)
{
	asmCode << "EXIT\n";
	asmCode << "MAIN ENDP\n";
	asmCode << "END MAIN\n";
}

void writePrintStatement(const string& stringVarName, stringstream& asmCode)
{
	asmCode << "MOV EDX, OFFSET " << stringVarName << "\n";
	asmCode << "CALL WriteString\n";
	asmCode << "CALL CRLF\n";
}

void writeAsmToFile(const char* fileName, stringstream& asmCode)
{
	ofstream output(fileName);

	output << asmCode.str();

	output.close();
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "Wrong number of arguments provided. Usage: compiler <Source File> <Compiled Output>" << endl;
		exit(EXIT_FAILURE);
	}

	ifstream inputFile(argv[1]); // Opens the file provided as first argument in Read Mode

	if (!inputFile.is_open())
	{
		cout << "ERROR: Cannot open input file. Exiting ..." << endl;
		exit(EXIT_FAILURE);
	}

	string currLine;

	// Reading the file and adding all the strings to the variables map
	int printCounts = 0;

	while (getline(inputFile, currLine))
	{
		string token = firstWord(currLine);

		if (token == "PRINT")
		{
			printCounts++;
			string variableName = "__p" + to_string(printCounts);

			string* val = new string(currLine.substr(currLine.find_first_of('"')));

			variables[make_pair(variableName, String)] = (void*)val;
		}
	}

	stringstream asmCode;

	// Checks if any error occurred during the process
	if(!writeIncludesAndDataSection(asmCode))
	{
		cout << "Invalid data type found in variables map. EXITING" << endl;
		inputFile.close();
		exit(EXIT_FAILURE);
	}

	// Reset file pointer to start
	inputFile.clear();
	inputFile.seekg(0, ios::beg);

	// Read the file again, this time writing to code segment of assembly file
	startCodeSection(asmCode);

	printCounts = 0;
	int lineNumber = 0;
	while (getline(inputFile, currLine))
	{
		lineNumber++;
		string token = firstWord(currLine);

		if (token == "PRINT")
		{
			printCounts++;
			string variableName = "__p" + to_string(printCounts);

			writePrintStatement(variableName, asmCode);
		}

		else if (token == "COMMENT" || token.empty())
		{
			cout << "IGNORING A COMMENT" << endl;
			continue;
		}

		else
		{
			cout << "ERROR @ Line " + to_string(lineNumber) + ": Invalid token " + token << endl;
			inputFile.close();
			exit(EXIT_FAILURE);
		}
	}

	endCodeSection(asmCode);

	inputFile.close();

	writeAsmToFile((filenameWithoutExt(argv[1]) + ".asm").c_str(), asmCode);

	system(("Assembler\\ml /c /nologo /Fo\"" + filenameWithoutExt(argv[1]) + ".obj\" /I \"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".asm").c_str());
	cout << " Linking: " + filenameWithoutExt(argv[1]) + ".obj irvine32.lib kernel32.lib user32.lib" << endl;
	system(("Linker\\link /OUT:\"" + filenameWithoutExt(argv[2]) + ".exe\" \"irvine32.lib\" \"kernel32.lib\" \"user32.lib\" /NOLOGO /SUBSYSTEM:CONSOLE /LIBPATH:\"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".obj").c_str());
	cout << " Saving: " + filenameWithoutExt(argv[2]) + ".exe" << endl;
	cout << " Deleting: " + filenameWithoutExt(argv[1]) + ".obj" << endl;
	system(("del " +  filenameWithoutExt(argv[1]) + ".obj").c_str());

	cout << "\n DONE!" << endl;

	return 0;
}
