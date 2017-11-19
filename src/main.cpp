#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <utility>
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
ifstream inputFile;
ofstream asmFile;

string toString(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

string filenameWithoutExt(char* fileName)
{
	string final = fileName;
	
	int dotLocation = final.find_last_of(".");
	if (dotLocation != final.npos) final = final.substr(0, dotLocation);
	
	return final;
}

void writeIncludesAndDataSection()
{
	asmFile << "INCLUDE Irvine32.inc\n\n";
	asmFile << ".DATA\n";

	for (map< pair<string, DataType>, void* >::iterator it = variables.begin(); it != variables.end(); it++)
	{
		string type;
		bool strVal = false;
		
		switch(it -> first.second)
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
				cout << "Invalid data type found in map. EXITING" << endl;
				exit(EXIT_FAILURE);
				break;
		}
		
		if (strVal)
		{
			string data(*(string*)(it -> second));
			asmFile << (it -> first.first) << " " << type << " " << data << ", 0\n";
		}
		
		else
		{
			int data = *(int*)(it -> second);
			asmFile << (it -> first.first) << " " << type << " " << data << "\n";
		}
	}
}

void startCodeSection()
{
	asmFile << "\n";
	asmFile << ".CODE\n";
	asmFile << "MAIN PROC\n";
}

void endCodeSection()
{
	asmFile << "EXIT\n";
	asmFile << "MAIN ENDP\n";
	asmFile << "END MAIN\n";
}

void writePrintStatement(string stringVarName)
{
	asmFile << "MOV EDX, OFFSET " << stringVarName << "\n";
	asmFile << "CALL WriteString\n";
	asmFile << "CALL CRLF\n";
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "Wrong number of arguments provided. Usage: compiler <Source File> <Compiled Output>" << endl;
		exit(EXIT_FAILURE);
	}
	
	inputFile.open(argv[1]);
	
	if (!inputFile.is_open())
	{
		cout << "ERROR: Cannot open input file. Exiting ..." << endl;
		exit(EXIT_FAILURE);
	}
	
	string currLine;
	asmFile.open(string(filenameWithoutExt(argv[1]) + ".asm").c_str());
	
	// Reading the file and adding all the strings to the variables map
	int printCounts = 0;
	while (getline(inputFile, currLine))
	{
		if (currLine.compare(0, string("PRINT").size(), "PRINT") == 0)
		{
			printCounts++;
			string variableName = "__p" + toString(printCounts);
			
			string* val = new string(currLine.substr(currLine.find_first_of("\"")));
			
			variables[make_pair(variableName, String)] = (void*)val;
		}
	}
	
	writeIncludesAndDataSection();
	
	// Reset file pointer to start
	inputFile.clear();
	inputFile.seekg(0, ios::beg);
		
	// Read the file again, this time writing to code segment of assembly file
	startCodeSection();
	
	printCounts = 0;
	while (getline(inputFile, currLine))
	{
		if (currLine.compare(0, string("PRINT").size(), "PRINT") == 0)
		{
			printCounts++;
			string variableName = "__p" + toString(printCounts);
			
			writePrintStatement(variableName);
		}
	}
	
	endCodeSection();
	
	inputFile.close();
	asmFile.close();
	
	system(string("Assembler\\ml /c /nologo /Fo\"" + filenameWithoutExt(argv[1]) + ".obj\" /I \"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".asm").c_str());
	cout << " Linking: " + filenameWithoutExt(argv[1]) + ".obj irvine32.lib kernel32.lib user32.lib" << endl;
	system(string("Linker\\link /OUT:\"" + filenameWithoutExt(argv[2]) + ".exe\" \"irvine32.lib\" \"kernel32.lib\" \"user32.lib\" /NOLOGO /SUBSYSTEM:CONSOLE /LIBPATH:\"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".obj").c_str());
	cout << " Saving: " + filenameWithoutExt(argv[2]) + ".exe" << endl;
	cout << " Deleting: " + filenameWithoutExt(argv[1]) + ".obj" << endl;
	system(string("del " +  filenameWithoutExt(argv[1]) + ".obj").c_str());
	
	cout << "\n DONE!" << endl;
	
	return 0;
}
