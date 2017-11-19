#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

map <string, bool> globalVars;

string filenameWithoutExt(char* fileName)
{
	string final = fileName;
	
	int dotLocation = final.find_last_of(".");
	if (dotLocation != final.npos) final = final.substr(0, dotLocation);
	
	return final;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "Wrong number of arguments provided. Usage: compiler <Source File> <Compiled Output>" << endl;
		return -1;
	}
	
	ifstream inputFile(argv[1]);
	
	if (!inputFile.is_open())
	{
		cout << "ERROR: Cannot open input file. Exiting ..." << endl;
		return -2;
	}
	
	string currLine;
	
	while (getline(inputFile, currLine))
	{
		cout << currLine << endl;
	}
	
	cout << endl;
	
	inputFile.close();
	
	system(string("Assembler\\ml /c /nologo /Fo\"" + filenameWithoutExt(argv[1]) + ".obj\" /I \"Library\\Irvine\" code.asm").c_str());
	cout << " Linking: " + filenameWithoutExt(argv[1]) + ".obj irvine32.lib kernel32.lib user32.lib" << endl;
	system(string("Linker\\link /OUT:\"" + filenameWithoutExt(argv[2]) + ".exe\" \"irvine32.lib\" \"kernel32.lib\" \"user32.lib\" /NOLOGO /SUBSYSTEM:CONSOLE /LIBPATH:\"Library\\Irvine\" " + filenameWithoutExt(argv[1]) + ".obj").c_str());
	cout << " Saving: " + filenameWithoutExt(argv[2]) + ".exe" << endl;
	cout << " Deleting: " + filenameWithoutExt(argv[1]) + ".obj" << endl;
	system(string("del " +  filenameWithoutExt(argv[1]) + ".obj").c_str());
	
	cout << "\n DONE!" << endl;
	
	return 0;
}
