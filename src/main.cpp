#include <map>
#include <set>
#include <regex>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

// TODO: +, -, *, /, %
// TODO: <, >, <=, >=, ==, !=
// TODO: AND, OR, NOT
// TODO: Logical Conditions
// TODO: Functions
// TODO: Loops
// TODO: Nested Loops and Recursion
// TODO: Implement some basic stuff with it

/// Language Specs:
/// Case-Sensitive (All keywords are capital)
/// Strongly Typed (Can't change variable data type after first initialization)
/// Scoped Variables (Variables are only accessible within their definition-scope)
/// Variable Types: CHARACTER (UNSIGNED, 1 BYTE), INTEGER (SIGNED, 4 BYTE), STRING (UNSIGNED, 1 BYTE SEQUENCE), BOOLEAN (UNSIGNED, 1 BYTE)
/// STRING type variables can only be initialized at declaration stage, and values can't be changed after initialization
/// Variable names cannot start with __ (two underscores) or a digit, cannot have spaces, cannot repeat variable names
/// LET is used to declare/initialize a variable
/// After variable is declared using LET, SET is used for changing the value stored in it

using namespace std;

enum DataTypes
{
	IntegerType = 1,
	CharacterType = 2,
	StringType = 4,
	BooleanType = 8
};

map<string, DataTypes> variables;
set<string> reservedWords;

string toUpperCase(const string& str)
{
	string final(str);

	for (char& c : final)
	{
		c = (char)toupper(c);
	}

	return final;
}

bool fillReservedWords()
{
	ifstream reservedWordsFile("reserved.txt");

	if (!reservedWordsFile.is_open()) return false;

	string word;

	while (reservedWordsFile >> word)
	{
		reservedWords.insert(word);
	}

	return true;
}

bool isReservedWord(const string& word)
{
	return reservedWords.find(toUpperCase(word)) != reservedWords.end();
}

string getASMDataSize(const string& type)
{
	if (type == "CHARACTER" || type == "STRING" || type == "BOOLEAN") return "BYTE";
	else return "SDWORD";
}

string getTypeFromEnum(DataTypes type)
{
	switch(type)
	{
		case IntegerType: return "INTEGER";
		case CharacterType: return "CHARACTER";
		case StringType: return "STRING";
		case BooleanType: return "BOOLEAN";
	}
}

DataTypes getEnumFromType(const string& type)
{
	if (type == "INTEGER") return IntegerType;
	else if (type == "CHARACTER") return CharacterType;
	else if (type == "STRING") return StringType;
	else return BooleanType;
}

vector<string> tokenize(const string& line)
{
	vector<string> tokens;
	bool inString = false;
	string current;

	for (int i = 0; i < line.size(); i++)
	{
		if (line[i] == '"') inString = !inString;
		if ((!inString && line[i] == ' ') || i == line.size() - 1)
		{
			if (i == line.size() - 1) current += line[i];

			tokens.push_back(current);
			current.clear();
		}

		else current += line[i];
	}

	return tokens;
}

string escapeQuoteLiteral(const string& token)
{
	string final;

	final += '"';

	for (int i = 1; i < token.size() - 1; i++)
	{
		if (token[i] == '"') final += "\"\"";
		else final += token[i];
	}

	final += '"';

	return final;
}

string filenameWithoutExt(char* fileName)
{
	string withoutExt(fileName);
	return withoutExt.substr(0, withoutExt.find_last_of('.'));
}

// TODO: Implement ENTER, LEAVE instructions for procedures (to allow functions and recursive calls)
void startProc(const string& procName, stringstream& codeSection)
{
	codeSection << toUpperCase(procName) << " PROC\n";
}

void endProc(const string& procName, stringstream& codeSection)
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

void writePrintStatement(const string& varName, const string& toPrint, stringstream& dataSection, stringstream& codeSection, bool newLine)
{
	dataSection << varName << " BYTE " << toPrint << ", 0\n";

	codeSection << "MOV EDX, OFFSET " << varName << "\n";
	codeSection << "CALL WriteString\n";
	if (newLine) codeSection << "CALL CRLF\n";
}

void writePrintStatement(const string& varName, const string& type, stringstream& codeSection, bool newLine)
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

void writeLetStatement(const string& varName, const string& size, const string& initialValue, stringstream& dataSection)
{
	dataSection << varName << " " << size << " " << initialValue << "\n";
}

void writeLetStatement(const string& varName, const string& value, stringstream& dataSection)
{
	dataSection << varName << " BYTE " << value << ", 0\n";
}

void writeSetStatement(const string& varName, const string& value, stringstream& codeSection)
{
	codeSection << "MOV " << varName << ", " << value << "\n";
}

void writeAsmToFile(const char* fileName, stringstream& dataSection, stringstream& codeSection)
{
	ofstream asmFile(fileName);

	if (!asmFile.is_open())
	{
		cout << "An error occurred while writing assembly file." << endl;
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

bool isValidPrint(const vector<string>& tokens, int line)
{
	if (tokens.size() != 2)
	{
		cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to PRINT/PRINTLINE statement." << endl;
		return false;
	}

	if (tokens[1][0] != '"')
	{
		if (variables.find(tokens[1]) == variables.end())
		{
			cout << "ERROR @ Line " << line << ": Variable not declared." << endl;
			return false;
		}
	}

	else if (tokens[1][tokens[1].size() - 1] != '"')
	{
		cout << "ERROR @ Line " << line << ": Invalid string literal." << endl;
		return false;
	}

	return true;
}

// LET statement is defined as: LET <var> AS <type> [= <value>]
bool isValidLet(const vector<string>& tokens, int line)
{
	if (tokens.size() != 4 && tokens.size() != 6)
	{
		cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to LET statement." << endl;
		return false;
	}

	if (!regex_match(tokens[1], regex("^[_A-Za-z][A-Za-z]*$")))
	{
		cout << "ERROR @ Line " << line << ": Variable name is not allowed to be used." << endl;
		return false;
	}

	if (isReservedWord(tokens[1]))
	{
		cout << "ERROR @ Line " << line << ": Variable name is reserved by the compiler." << endl;
		return false;
	}

	if (tokens[2] != "AS")
	{
		cout << "ERROR @ Line " << line << ": Expected AS keyword." << endl;
		return false;
	}

	if (tokens[3] != "CHARACTER" && tokens[3] != "INTEGER" && tokens[3] != "STRING" && tokens[3] != "BOOLEAN")
	{
		cout << "ERROR @ Line " << line << ": Unknown/Unsupported data type provided." << endl;
		return false;
	}

	if (variables.find(tokens[1]) != variables.end())
	{
		cout << "ERROR @ Line " << line << ": Variable is already declared. Use SET statement to change it's value." << endl;
		return false;
	}

	if (tokens[3] == "STRING" && tokens.size() != 6)
	{
		cout << "ERROR @ Line " << line << ": STRING type variables must be initialized at the time of declaration." << endl;
		return false;
	}

	if (tokens.size() == 6 && tokens[4] != "=")
	{
		cout << "ERROR @ Line " << line << ": Expected = sign" << endl;
		return false;
	}

	if (tokens[3] == "STRING")
	{
		if (tokens[5][0] != '"' || tokens[5][tokens[5].size() - 1] != '"')
		{
			cout << "ERROR @ Line " << line << ": Invalid String literal." << endl;
			return false;
		}

		for (const auto& c : tokens[5])
		{
			if (!isascii(c))
			{
				cout << "ERROR @ Line " << line << ": Non-ASCII Strings are not supported." << endl;
				return false;
			}
		}
	}

	if (tokens.size() == 6)
	{
		if (tokens[3] == "INTEGER")
		{
			if (!regex_match(tokens[5], regex("^[+-]{0,1}\\d+$")))
			{
				cout << "ERROR @ Line " << line << ": Invalid value for an integer variable." << endl;
				return false;
			}
		}

		else if (tokens[3] == "CHARACTER")
		{
			if (tokens[5].size() != 3 || tokens[5][0] != '\'' || tokens[5][2] != '\'')
			{
				cout << "ERROR @ Line " << line << ": Invalid Character literal." << endl;
				return false;
			}

			if (!isascii(tokens[5][1]))
			{
				cout << "ERROR @ Line " << line << ": Non-ASCII Characters are not supported." << endl;
				return false;
			}
		}

		else if (tokens[3] == "BOOLEAN")
		{
			if (tokens[5] != "TRUE" || tokens[5] != "FALSE")
			{
				cout << "ERROR @ Line " << line << ": Invalid value for a boolean variable." << endl;
				return false;
			}
		}
	}

	return true;
}

// SET statement is defined as: SET <var> = <value>
bool isValidSet(const vector<string>& tokens, int line)
{
	if (tokens.size() != 4)
	{
		cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to SET statement." << endl;
		return false;
	}

	if (variables.find(tokens[1]) == variables.end())
	{
		cout << "ERROR @ Line " << line << ": Variable not declared." << endl;
		return false;
	}

	if (tokens[2] != "=")
	{
		cout << "ERROR @ Line " << line << ": Expected '='." << endl;
		return false;
	}

	if (variables[tokens[1]] == StringType)
	{
		cout << "ERROR @ Line " << line << ": Changing value of STRING type variables is not supported." << endl;
		return false;
	}

	else if (variables[tokens[1]] == IntegerType)
	{
		if (!regex_match(tokens[3], regex("^[+-]{0,1}\\d+$")))
		{
			cout << "ERROR @ Line " << line << ": Invalid value for an integer variable." << endl;
			return false;
		}
	}

	else if (variables[tokens[1]] == CharacterType)
	{
		if (tokens[3].size() != 3 || tokens[3][0] != '\'' || tokens[3][2] != '\'')
		{
			cout << "ERROR @ Line " << line << ": Invalid Character literal." << endl;
			return false;
		}

		if (!isascii(tokens[3][1]))
		{
			cout << "ERROR @ Line " << line << ": Non-ASCII Characters are not supported." << endl;
			return false;
		}
	}

	else if (variables[tokens[1]] == BooleanType)
	{
		if (tokens[3] != "TRUE" || tokens[3] != "FALSE")
		{
			cout << "ERROR @ Line " << line << ": Invalid value for a boolean variable." << endl;
			return false;
		}
	}

	return true;
}

// PAUSE statement is defined as: PAUSE
bool isValidPause(const vector<string>& tokens, int line)
{
	if (tokens.size() != 1)
	{
		cout << "ERROR @ Line " << line << ": PAUSE statement does not take any parameters." << endl;
		return false;
	}

	return true;
}

// NEWLINE statement is defined as: NEWLINE
bool isValidNewLine(const vector<string>& tokens, int line)
{
	if (tokens.size() != 1)
	{
		cout << "ERROR @ Line " << line << ": NEWLINE statement does not take any parameters." << endl;
		return false;
	}

	return true;
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

	if (!fillReservedWords())
	{
		cout << "ERROR: Cannot open reserved words file. Exiting ..." << endl;
		exit(EXIT_FAILURE);
	}

	stringstream dataSection;
	stringstream codeSection;

	string currLine;

	// Reading the file and adding all the strings to the variables map
	int printCounts = 0;
	int lineNumber = 0;

	startProc("MAIN", codeSection);

	while (getline(inputFile, currLine))
	{
		lineNumber++;
		vector<string> tokens = tokenize(currLine);

		if (tokens.empty() || tokens[0].empty() || tokens[0] == "COMMENT")
		{
			// Ignoring empty/commented lines
		}

		else if (tokens[0] == "PRINT" || tokens[0] == "PRINTLINE")
		{
			if (!isValidPrint(tokens, lineNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			if (tokens[1][0] == '"')
			{
				printCounts++;
				string varName = "__p" + to_string(printCounts);
				writePrintStatement(varName, escapeQuoteLiteral(tokens[1]), dataSection, codeSection, tokens[0] == "PRINTLINE");
			}

			else writePrintStatement(tokens[1], getTypeFromEnum(variables[tokens[1]]), codeSection, tokens[0] == "PRINTLINE");
		}

		else if (tokens[0] == "LET")
		{
			if (!isValidLet(tokens, lineNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			string varName = tokens[1];
			variables[varName] = getEnumFromType(tokens[3]);

			if (tokens[3] == "STRING") writeLetStatement(varName, escapeQuoteLiteral(tokens[5]), dataSection);
			else writeLetStatement(varName, getASMDataSize(tokens[3]), tokens.size() == 6 ? tokens[5] : "?", dataSection);
		}

		else if (tokens[0] == "SET")
		{
			if (!isValidSet(tokens, lineNumber))
			{
				inputFile.close();
				exit(EXIT_FAILURE);
			}

			string varName = tokens[1];
			string value = tokens[3];
			writeSetStatement(varName, value, codeSection);
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
			cout << "ERROR @ Line " << lineNumber << ": Statement is not a part of syntax." << endl;
			inputFile.close();
			exit(EXIT_FAILURE);
		}
	}

	endProc("MAIN", codeSection);

	inputFile.close();

	writeAsmToFile((filenameWithoutExt(argv[1]) + ".asm").c_str(), dataSection, codeSection);

	system(("Assembler\\ml /c /nologo /Fo\"" + filenameWithoutExt(argv[1]) + ".obj\" /I \"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".asm").c_str());
	cout << " Linking: " + filenameWithoutExt(argv[1]) + ".obj irvine32.lib kernel32.lib user32.lib" << endl;
	system(("Linker\\link /OUT:\"" + filenameWithoutExt(argv[2]) + ".exe\" \"irvine32.lib\" \"kernel32.lib\" \"user32.lib\" /NOLOGO /SUBSYSTEM:CONSOLE /LIBPATH:\"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".obj").c_str());
	cout << " Saving: " + filenameWithoutExt(argv[2]) + ".exe" << endl;
	cout << " Deleting: " + filenameWithoutExt(argv[1]) + ".obj" << endl;
	system(("del " +  filenameWithoutExt(argv[1]) + ".obj").c_str());

	cout << "\n DONE!" << endl;

	// Runs the compiled executable
	system("cls");
	system("out.exe");

	return 0;
}
