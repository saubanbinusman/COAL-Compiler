#include "Reserved.h"
#include "StringFunctions.h"

std::set<std::string> reservedWords;

bool fillReservedWords()
{
	std::ifstream reservedWordsFile("reserved.txt");

	if (!reservedWordsFile.is_open()) return false;

	std::string word;

	while (reservedWordsFile >> word)
	{
		reservedWords.insert(word);
	}

	return true;
}

bool isReservedWord(const std::string& word)
{
	return reservedWords.find(toUpperCase(word)) != reservedWords.end();
}
