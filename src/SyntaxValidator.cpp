#include "Reserved.h"
#include "VariablesMap.h"
#include "StringFunctions.h"
#include "SyntaxValidator.h"

// PRINT statement is defined: [PRINT OR PRINTLINE] [<string literal> OR <variable name>]
bool isValidPrint(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	if (tokens.size() != 2)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to PRINT/PRINTLINE statement." << std::endl;
		return false;
	}

	if (tokens[1][0] != '"')
	{
		if (!isDefinedInAccessibleScopes(tokens[1], upperScopes))
		{
			std::cout << "ERROR @ Line " << line << ": Variable not declared." << std::endl;
			return false;
		}
	}

	else if (tokens[1][tokens[1].size() - 1] != '"')
	{
		std::cout << "ERROR @ Line " << line << ": Invalid string literal." << std::endl;
		return false;
	}

	return true;
}

// LET statement is defined as: LET <var> AS <type> [= <value>]
bool isValidLet(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	if (tokens.size() != 4 && tokens.size() != 6)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to LET statement." << std::endl;
		return false;
	}

	if (!regex_match(tokens[1], std::regex("^[_A-Za-z][A-Za-z]*$")))
	{
		std::cout << "ERROR @ Line " << line << ": Variable name is not allowed to be used." << std::endl;
		return false;
	}

	if (isReservedWord(tokens[1]))
	{
		std::cout << "ERROR @ Line " << line << ": Variable name is reserved by the compiler." << std::endl;
		return false;
	}

	if (tokens[2] != "AS")
	{
		std::cout << "ERROR @ Line " << line << ": Expected AS keyword." << std::endl;
		return false;
	}

	if (tokens[3] != "CHARACTER" && tokens[3] != "INTEGER" && tokens[3] != "STRING" && tokens[3] != "BOOLEAN")
	{
		std::cout << "ERROR @ Line " << line << ": Unknown/Unsupported data type provided." << std::endl;
		return false;
	}

	if (isDefinedInAccessibleScopes(tokens[1], upperScopes))
	{
		std::cout << "ERROR @ Line " << line << ": Variable is already declared. Use SET statement to change it's value." << std::endl;
		return false;
	}

	if (tokens[3] == "STRING" && tokens.size() != 6)
	{
		std::cout << "ERROR @ Line " << line << ": STRING type variables must be initialized at the time of declaration." << std::endl;
		return false;
	}

	if (tokens.size() == 6 && tokens[4] != "=")
	{
		std::cout << "ERROR @ Line " << line << ": Expected '=' sign." << std::endl;
		return false;
	}

	if (tokens[3] == "STRING")
	{
		if (tokens[5][0] != '"' || tokens[5][tokens[5].size() - 1] != '"')
		{
			std::cout << "ERROR @ Line " << line << ": Invalid String literal." << std::endl;
			return false;
		}

		for (const auto& c : tokens[5])
		{
			//if (!isascii(c))
			if (false)
			{
				std::cout << "ERROR @ Line " << line << ": Non-ASCII Strings are not supported." << std::endl;
				return false;
			}
		}
	}

	if (tokens.size() == 6)
	{
		if (tokens[3] == "INTEGER")
		{
			if (!regex_match(tokens[5], std::regex("^[+-]{0,1}\\d+$")))
			{
				std::cout << "ERROR @ Line " << line << ": Invalid value for an integer variable." << std::endl;
				return false;
			}
		}

		else if (tokens[3] == "CHARACTER")
		{
			if (tokens[5].size() != 3 || tokens[5][0] != '\'' || tokens[5][2] != '\'')
			{
				std::cout << "ERROR @ Line " << line << ": Invalid Character literal." << std::endl;
				return false;
			}

			//if (!isascii(tokens[5][1]))
			if (false)
			{
				std::cout << "ERROR @ Line " << line << ": Non-ASCII Characters are not supported." << std::endl;
				return false;
			}
		}

		else if (tokens[3] == "BOOLEAN")
		{
			if (tokens[5] != "TRUE" && tokens[5] != "FALSE")
			{
				std::cout << "ERROR @ Line " << line << ": Invalid value for a boolean variable." << std::endl;
				return false;
			}
		}
	}

	return true;
}

// SET statement is defined as: SET <var> = <value>
bool isValidSet(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	if (tokens.size() < 4)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to SET statement." << std::endl;
		return false;
	}

	if (!isDefinedInAccessibleScopes(tokens[1], upperScopes))
	{
		std::cout << "ERROR @ Line " << line << ": Variable not declared." << std::endl;
		return false;
	}

	if (tokens[2] != "=")
	{
		std::cout << "ERROR @ Line " << line << ": Expected '='." << std::endl;
		return false;
	}

	if ((variables[tokens[1]].type != IntegerType && variables[tokens[1]].type != BooleanType) && tokens.size() != 4)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to SET statement." << std::endl;
		return false;
	}

	if (variables[tokens[1]].type == StringType)
	{
		std::cout << "ERROR @ Line " << line << ": Changing value of STRING type variables is not supported." << std::endl;
		return false;
	}

	else if (variables[tokens[1]].type == IntegerType)
	{
		std::string val = tokens[3];
		for (int i = 4; i < tokens.size(); i++) val += " " + tokens[i];

		if (!regex_match(removeSpacesAndBrackets(val), std::regex("^([+\\-]?([\\d]+|[\\w]+))([*/\\^+\\-][+\\-]*([\\d]+|[\\w]+))*")))
		{
			std::cout << "ERROR @ Line " << line << ": Invalid expression or value for integer variable => " << val << std::endl;
			return false;
		}
	}

	else if (variables[tokens[1]].type == CharacterType)
	{
		if (tokens[3].size() != 3 || tokens[3][0] != '\'' || tokens[3][2] != '\'')
		{
			std::cout << "ERROR @ Line " << line << ": Invalid Character literal." << std::endl;
			return false;
		}

		// Disabled because older MinGW wasnt supporting it on laptop
		//if (!isascii(tokens[3][1]))
		if (false)
		{
			std::cout << "ERROR @ Line " << line << ": Non-ASCII Characters are not supported." << std::endl;
			return false;
		}
	}

	else if (variables[tokens[1]].type == BooleanType)
	{
		std::string val = tokens[3];
		for (int i = 4; i < tokens.size(); i++) val += " " + tokens[i];

		if (!regex_match(removeBrackets(val), std::regex("^\\s*(((NOT)*[\\s]+)*[\\w]+)((([\\s]*(AND|OR)[\\s]+((NOT)*[\\s]+)*)|([\\s]*(<|<=|>|>=|==|!=)[\\s]*))[\\w]+)*")))
		{
			std::cout << "ERROR @ Line " << line << ": Invalid expression or value for boolean variable => " << val << std::endl;
			return false;
		}
	}

	return true;
}

// INPUT statement is defined as: INPUT <var>
bool isValidInput(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	if (tokens.size() != 2)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to INPUT statement." << std::endl;
		return false;
	}

	if (!isDefinedInAccessibleScopes(tokens[1], upperScopes))
	{
		std::cout << "ERROR @ Line " << line << ": Variable not declared." << std::endl;
		return false;
	}

	if (getVariableType(tokens[0]) == StringType)
	{
		std::cout << "ERROR @ Line " << line << ": Changing value of STRING type variables is not supported." << std::endl;
		return false;
	}

	return true;
}

// IF statement is defined as: IF (<logical expr>)
bool isValidIf(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	std::string s;

	for (int i = 1; i < tokens.size(); i++) s += tokens[i];

	if (!std::regex_match(s, std::regex("(^\\().*(\\)$)")))
	{
		std::cout << "ERROR @ Line " << line << ": The logical expression is not properly parenthesized." << std::endl;
		return false;
	}

	return true;
}

// ELSEIF statement is defined as: ELSEIF (<logical expr>)
bool isValidElseIf(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	std::string s;

	for (int i = 1; i < tokens.size(); i++) s += tokens[i];

	if (!std::regex_match(s, std::regex("(^\\().*(\\)$)")))
	{
		std::cout << "ERROR @ Line " << line << ": The logical expression is not properly parenthesized." << std::endl;
		return false;
	}

	return true;
}

bool isValidEndIf(const std::vector<std::string>& tokens, const int& line, const std::string& scope, const int& scopeNumber)
{
	if (tokens.size() != 1)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid ENDIF Syntax." << std::endl;
		return false;
	}

	if (scope != std::to_string(scopeNumber) + "IF")
	{
		std::cout << "Error @ Line " << line << ": Invalid ENDIF without matching IF statement." << std::endl;
		return false;
	}

	return true;
}

// PAUSE statement is defined as: PAUSE
bool isValidPause(const std::vector<std::string>& tokens, const int& line)
{
	if (tokens.size() != 1)
	{
		std::cout << "ERROR @ Line " << line << ": PAUSE statement does not take any parameters." << std::endl;
		return false;
	}

	return true;
}

// NEWLINE statement is defined as: NEWLINE
bool isValidNewLine(const std::vector<std::string>& tokens, const int& line)
{
	if (tokens.size() != 1)
	{
		std::cout << "ERROR @ Line " << line << ": NEWLINE statement does not take any parameters." << std::endl;
		return false;
	}

	return true;
}

bool isValidLogicalExpression(std::deque<std::string> tokens, const int& line)
{
	std::stack<std::string> stackSimulator;

	while (!tokens.empty())
	{
		if (tokens.front() == "NOT")
		{
			std::string temp = stackSimulator.top();
			stackSimulator.pop();

			if (temp != "TRUE" && temp != "FALSE" && getVariableType(temp) != BooleanType)
			{
				std::cout << "ERROR @ Line " << line << ": Cannot apply NOT to resolved types." << std::endl;
				return false;
			}

			stackSimulator.push("TRUE");
		}

		else if (tokens.front() == "AND" || tokens.front() == "OR")
		{
			for (int i = 0; i < 2; i++)
			{
				std::string temp = stackSimulator.top();
				stackSimulator.pop();

				if (temp != "TRUE" && temp != "FALSE" && getVariableType(temp) != BooleanType)
				{
					std::cout << "ERROR @ Line " << line << ": Cannot apply Logical AND,OR to resolved types." << std::endl;
					return false;
				}
			}

			stackSimulator.push("TRUE");
		}

		else if (logical_IsComparator(tokens.front()))
		{
			for (int i = 0; i < 2; i++)
			{
				std::string temp = stackSimulator.top();
				stackSimulator.pop();

				if (!isNumericValue(temp) && getVariableType(temp) != IntegerType)
				{
					std::cout << "ERROR @ Line " << line << ": Cannot apply comparator to resolved types." << std::endl;
					return false;
				}
			}

			stackSimulator.push("TRUE");
		}

		else
		{
			stackSimulator.push(tokens.front());
		}

		tokens.pop_front();
	}

	stackSimulator.pop();

	if (!stackSimulator.empty())
	{
		std::cout << "ERROR @ Line " << line << ": Invalid Expression, Stack not empty after expression resolution." << std::endl;
		return false;
	}

	return true;
}