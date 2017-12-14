#include "Reserved.h"
#include "DefinitionsMap.h"
#include "SyntaxValidator.h"

// PRINT statement is defined: [PRINT OR PRINTLINE] [<string literal> OR <variable name>]
bool isValidPrint(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	if (tokens.size() != 2)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to PRINT/PRINTLINE statement." << std::endl;
		return false;
	}

	if (upperScopes.top() == "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": PRINT statement cannot be used outside a METHOD." << std::endl;
		return false;
	}

	if (tokens[1][0] != '"')
	{
		if (!isVariableDefinedInAccessibleScopes(tokens[1], upperScopes))
		{
			std::cout << "ERROR @ Line " << line << ": Variable is not accessible in current scope." << std::endl;
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

// LET statement is defined as: LET <var> AS <type> [= <literal value>]
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

	if (isVariableDefinedInAccessibleScopes(tokens[1], upperScopes))
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
		if (!std::regex_match(tokens[5], std::regex("^\"[\\x20-\\x7E]*\"")))
		{
			std::cout << "ERROR @ Line " << line << ": Invalid String literal." << std::endl;
			return false;
		}
	}

	if (tokens.size() == 6)
	{
		if (tokens[3] == "INTEGER")
		{
			if (!regex_match(tokens[5], std::regex("^[+-]?\\d+")))
			{
				std::cout << "ERROR @ Line " << line << ": Invalid value for an integer variable." << std::endl;
				return false;
			}
		}

		else if (tokens[3] == "CHARACTER")
		{
			if (!std::regex_match(tokens[5], std::regex("^'[\\x20-\\x7E]'")))
			{
				std::cout << "ERROR @ Line " << line << ": Invalid Character literal." << std::endl;
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

	if (!isVariableDefinedInAccessibleScopes(tokens[1], upperScopes))
	{
		std::cout << "ERROR @ Line " << line << ": Variable not accessible in current scope OR is not declared." << std::endl;
		return false;
	}

	if (tokens[2] != "=")
	{
		std::cout << "ERROR @ Line " << line << ": Expected '='." << std::endl;
		return false;
	}

	if (getVariableObject(tokens[1]).type == StringType)
	{
		std::cout << "ERROR @ Line " << line << ": Changing value of STRING type variables is not supported." << std::endl;
		return false;
	}

	if (tokens[3] == "CALL")
	{
		std::string callLine = tokens[3];
		for (int i = 4; i < tokens.size(); i++)
		{
			callLine += " " + tokens[i];
		}

		if (!isValidCall(tokenize(removeLeadingSpace(callLine)), removeLeadingSpace(callLine), line, upperScopes))
		{
			return false;
		}

		std::vector<std::string> callTokens = regexTokenize(callLine, std::regex("[^\\s(,)]+"));
		if (getVariableType(tokens[1]) != getMethodReturnType(callTokens[1]))
		{
			std::cout << "ERROR @ Line " << line << ": The Data Type of Variable to be SET and the return type of CALLED function did not match." << std::endl;
			return false;
		}
	}

	else
	{
		if (getVariableType(tokens[1]) == CharacterType)
		{
			if (tokens.size() != 4)
			{
				std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to SET statement." << std::endl;
				return false;
			}

			if (tokens[5][0] != '\'')
			{
				if (!isVariableDefinedInAccessibleScopes(tokens[5], upperScopes))
				{
					std::cout << "Variable is not accessible in current scope OR is not declared." << std::endl;
					return false;
				}
			}

			else if (!std::regex_match(tokens[5], std::regex("^'[\\x20-\\x7E]'")))
			{
				std::cout << "ERROR @ Line " << line << ": Invalid Character literal." << std::endl;
				return false;
			}
		}

		else if (getVariableObject(tokens[1]).type == IntegerType)
		{
			std::string val = tokens[3];
			for (int i = 4; i < tokens.size(); i++) val += " " + tokens[i];

			if (!regex_match(removeSpacesAndBrackets(val), std::regex("^([+\\-]?([\\d]+|[\\w]+))([*/\\^+\\-][+\\-]*([\\d]+|[\\w]+))*")))
			{
				std::cout << "ERROR @ Line " << line << ": Invalid expression or value for integer variable => " << val << std::endl;
				return false;
			}
		}

		else if (getVariableObject(tokens[1]).type == BooleanType)
		{
			std::string val = tokens[3];
			for (int i = 4; i < tokens.size(); i++) val += " " + tokens[i];

			if (!regex_match(removeBrackets(val), std::regex("^\\s*(((NOT)*[\\s]+)*[\\w]+)((([\\s]*(AND|OR)[\\s]+((NOT)*[\\s]+)*)|([\\s]*(<|<=|>|>=|==|!=)[\\s]*))[\\w]+)*")))
			{
				std::cout << "ERROR @ Line " << line << ": Invalid expression or value for boolean variable => " << val << std::endl;
				return false;
			}
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

	if (upperScopes.top() == "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": INPUT statement cannot be used outside a METHOD." << std::endl;
		return false;
	}

	if (!isVariableDefinedInAccessibleScopes(tokens[1], upperScopes))
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
	if (upperScopes.top() == "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": IF statement cannot be used outside a METHOD." << std::endl;
		return false;
	}

	std::string s;

	for (int i = 1; i < tokens.size(); i++) s += tokens[i];

	if (!std::regex_match(s, std::regex("(^\\().*(\\)$)")))
	{
		std::cout << "ERROR @ Line " << line << ": The logical expression is not properly parenthesized." << std::endl;
		return false;
	}

	return true;
}

// ENDIF statement is defined as: ENDIF
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

// METHOD statement is defined as: METHOD <name> [TAKES <var1> AS <type>, <var2> AS <type> ...] RETURNS <BOOLEAN, CHARACTER, INTEGER, NOTHING>
bool isValidMethod(const std::vector<std::string>& tokens, const std::string& completeLine, const int& line, const std::string& scope)
{
	if (tokens.size() < 4)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to METHOD statement." << std::endl;
		return false;
	}

	if (scope != "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": Nested METHOD statements are not possible." << std::endl;
		return false;
	}

	if (!std::regex_match(completeLine, std::regex("^METHOD\\s+\\w+\\s+(TAKES\\s+(\\s*\\w+\\s+AS\\s+(INTEGER|STRING|CHARACTER|BOOLEAN)(\\s+REF)?\\s*,\\s*)*)?RETURNS\\s+(INTEGER|CHARACTER|BOOLEAN|NOTHING)")))
	{
		std::cout << "ERROR @ Line " << line << ": Invalid METHOD statement. It did not qualify Regex test." << std::endl;
		return false;
	}

	if (isMethodDefined(tokens[1]))
	{
		std::cout << "ERROR @ Line " << line << ": Method is already defined." << std::endl;
		return false;
	}

	return true;
}

// ENDMETHOD statement is defined as: ENDMETHOD
bool isValidEndMethod(const std::vector<std::string>& tokens, const int& line, const std::string& scope)
{
	if (tokens.size() != 1)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to ENDMETHOD statement." << std::endl;
		return false;
	}

	if (scope == "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": ENDMETHOD statement found without a matching METHOD statement." << std::endl;
		return false;
	}

	if (!isMethodDefined(scope))
	{
		std::cout << "ERROR @ Line " << line << ": Invalid scope. Please END previous scoping statements before ENDMETHOD." << std::endl;
		return false;
	}

	if (!isMethodReturned(scope))
	{
		// std::cout << "WARNING @ Line " << line << ": Method does not have a RETURN statement. Expect a default return value." << std::endl;
	}

	return true;
}

// CALL statement is defined as: CALL <method>[(param1, param2, param3)]...
bool isValidCall(const std::vector<std::string>& tokens, const std::string& completeLine, const int& line, const std::stack<std::string>& upperScopes)
{
	if (tokens.size() < 2)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to CALL statement." << std::endl;
		return false;
	}

	if (upperScopes.top() == "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": CALL statement cannot be used outside a METHOD." << std::endl;
		return false;
	}

	if (!std::regex_match(completeLine, std::regex("^CALL\\s+\\w+(\\s*\\((\\s*[-+\\'\\w]+,)*(\\s*[-+\\'\\w]+)\\))*")))
	{
		std::cout << "ERROR @ Line " << line << ": Invalid CALL statement. It did not qualify Regex test." << std::endl;
		return false;
	}

	const std::vector<std::string>& callTokens = regexTokenize(removeLeadingSpace(completeLine), std::regex("[^\\s(,)]+"));
	const std::vector<Parameter>& params = getMethodParameters(callTokens[1]);

	if (params.size() != callTokens.size() - 2)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of arguments passed to method. Required: " << params.size() << ", Passed: " << callTokens.size() - 2 << "." << std::endl;
		return false;
	}

	if (!isMethodDefined(callTokens[1]))
	{
		std::cout << "ERROR @ Line " << line << ": Method is not defined." << std::endl;
		return false;
	}

	for (int i = 2; i < callTokens.size(); i++)
	{
		if (isVariableDefinedInAccessibleScopes(callTokens[i], upperScopes))
		{
			if (params[i - 2].type != getVariableObject(callTokens[i]).type)
			{
				std::cout << "ERROR @ Line " << line << ": Data Types did not match." << std::endl;
				return false;
			}
		}

		else
		{
			if (params[i - 2].reference)
			{
				std::cout << "ERROR @ Line " << line << ": Literal Values are not allowed for pass by reference calling." << std::endl;
				return false;
			}

			if (params[i - 2].type != getEnumFromValue(callTokens[i]))
			{
				std::cout << "ERROR @ Line " << line << ": Data Types did not match." << std::endl;
				return false;
			}
		}
	}

	return true;
}

// RETURN statement is defined as: RETURN [<value>]
bool isValidReturn(const std::vector<std::string>& tokens, const int& line, const std::stack<std::string>& upperScopes)
{
	if (tokens.size() > 2)
	{
		std::cout << "ERROR @ Line " << line << ": Invalid number of parameters supplied to RETURN statement." << std::endl;
		return false;
	}

	if (upperScopes.top() == "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": RETURN statement cannot be used outside a METHOD." << std::endl;
		return false;
	}

	if (getMethodInCurrentScope(upperScopes).empty())
	{
		std::cout << "ERROR @ Line " << line << ": Could not find enclosing METHOD." << std::endl;
		return false;
	}

	if (getMethodReturnType(getMethodInCurrentScope(upperScopes)) == UndefinedType && tokens.size() == 2)
	{
		std::cout << "ERROR @ Line " << line << ": Cannot RETURN data from a METHOD which returns NOTHING." << std::endl;
		return false;
	}

	if (isNumericValue(tokens[1]) || !isVariableDefinedInAccessibleScopes(tokens[1], upperScopes))
	{
		if (getEnumFromValue(tokens[1]) == UndefinedType || getEnumFromValue(tokens[1]) == StringType)
		{
			std::cout << "ERROR @ Line " << line << ": Invalid data. Can only return a variable or a literal (excluding STRING type)." << std::endl;
			return false;
		}

		if (getEnumFromValue(tokens[1]) != getMethodReturnType(getMethodInCurrentScope(upperScopes)))
		{
			std::cout << "ERROR @ Line " << line << ": Invalid data. Data type did not match the method return type." << std::endl;
			return false;
		}
	}

	else
	{
		if (getVariableType(tokens[1]) != getMethodReturnType(getMethodInCurrentScope(upperScopes)))
		{
			std::cout << "ERROR @ Line " << line << ": Invalid data. Data type did not match the method return type." << std::endl;
			return false;
		}

		if (getVariableType(tokens[1]) == StringType)
		{
			std::cout << "ERROR @ Line " << line << ": Cannot return STRING type data." << std::endl;
			return false;
		}
	}

	return true;
}

// PAUSE statement is defined as: PAUSE
bool isValidPause(const std::vector<std::string>& tokens, const int& line, const std::string& scope)
{
	if (tokens.size() != 1)
	{
		std::cout << "ERROR @ Line " << line << ": PAUSE statement does not take any parameters." << std::endl;
		return false;
	}

	if (scope == "GLOBAL")
	{
		std::cout << "ERROR @ Line " << line << ": PAUSE statement cannot be used outside a METHOD." << std::endl;
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