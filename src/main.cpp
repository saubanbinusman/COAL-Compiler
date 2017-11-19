#include <iostream>
#include <fstream>
#include <map>

using namespace std;

map <string, bool> globalVars;

int main(int argc, char** argv)
{
	if (argc == 0 || argc == 1)
	{
		cout << "Wrong number of arguments provided. Usage: compiler <Source File> <Compiled Output>" << endl;
		return -1;
	}
	
	string toCompile = argv[1];
	ifstream inputFile;
	inputFile.open(toCompile.c_str());
	
	if (!inputFile.is_open())
	{
		cout << "ERROR: Cannot open input file. Exiting ..." << endl;
		return -1;
	}
	
	string currLine;
	
	while (inputFile >> currLine)
	{
		cout << currLine << endl;
	}
	
	return 0;
}
