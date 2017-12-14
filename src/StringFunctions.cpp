#include "StringFunctions.h"

std::string toUpperCase(const std::string& str)
{
	std::string final(str);

	for (char& c : final)
	{
		c = (char)toupper(c);
	}

	return final;
}

std::string removeLeadingSpace(const std::string& str)
{
	int lineIt;
	for (lineIt = 0; lineIt < str.size(); lineIt++)
	{
		const char& c = str[lineIt];
		if (c == '\t' || c == ' ' || c == '\n' || c == '\r') continue;
		break;
	}

	std::string trimmedLine;

	for ( ; lineIt < str.size(); lineIt++)
	{
		trimmedLine += str[lineIt];
	}

	return trimmedLine;
}

bool arithmetic_IsOperator(const std::string& s)
{
	return (s == "+" || s == "-" || s == "*" || s == "/" || s == "%" || s == "^" || s == "__p" || s == "__m");
}

int arithmetic_GetPrecedence(const std::string& s)
{
	if (s == "^" || s == "__p" || s == "__m") return 30;
	if (s == "*" || s == "/" || s == "%") return 20;
	if (s == "+" || s == "-") return 10;
}

bool arithmetic_IsLeftAssociative(const std::string& s)
{
	return !(s == "^" || s == "__m" || s == "__p");
}

std::vector<std::string> arithmetic_ExpressionTokenizer(const std::string& expression)
{
	std::vector<std::string> tokens;
	std::string currentToken;

	for (int i = 0; i < expression.size(); i++)
	{
		if (i == expression.size() - 1)
		{
			if (arithmetic_IsOperator(std::string(1, expression[i])) || expression[i] == '(' || expression[i] == ')')
			{
				if (!currentToken.empty()) tokens.push_back(currentToken);
				tokens.push_back(std::string(1, expression[i]));
			}

			else
			{
				currentToken += expression[i];
				tokens.push_back(currentToken);
			}
		}

		else if (arithmetic_IsOperator(std::string(1, expression[i])) || expression[i] == '(' || expression[i] == ')')
		{
			if (!currentToken.empty()) tokens.push_back(currentToken);
			currentToken.clear();

			if (expression[i] == '+' || expression[i] == '-')
			{
				// Checking if it's a unary + or -
				if (i != 0 && (arithmetic_IsOperator(std::string(1, expression[i - 1])) || expression[i - 1] == '('))
				{
					if (expression[i] == '+') tokens.push_back("__p");
					else tokens.push_back("__m");
				}

				else if (i == 0)
				{
					if (expression[i] == '+') tokens.push_back("__p");
					else tokens.push_back("__m");
				}

				else
				{
					tokens.push_back(std::string(1, expression[i]));
				}
			}

			else tokens.push_back(std::string(1, expression[i]));
		}

		else currentToken += expression[i];
	}

	return tokens;
}

std::deque<std::string> arithmetic_InfixToPostfix(const std::string& expression, const int& line, const std::stack<std::string>& upperScopes)
{
	std::string trimmedExpr = removeSpaces(expression);
	std::vector<std::string> tokens = arithmetic_ExpressionTokenizer(trimmedExpr);

	std::stack<std::string> opStack;
	std::deque<std::string> postfixDeque;

	// Parse Infix to Postfix
	for (const auto& token : tokens)
	{
		if (isNumericValue(token)) postfixDeque.push_back(token);

		else if (arithmetic_IsOperator(token))
		{
			while (!opStack.empty() && ((arithmetic_GetPrecedence(opStack.top()) > arithmetic_GetPrecedence(token)) || ((arithmetic_GetPrecedence(opStack.top()) == arithmetic_GetPrecedence(token)) && arithmetic_IsLeftAssociative(token)) && opStack.top() != "("))
			{
				postfixDeque.push_back(opStack.top());
				opStack.pop();
			}

			opStack.push(token);
		}

		else if (token == "(") opStack.push("(");

		else if (token == ")")
		{
			while (!opStack.empty() && opStack.top() != "(")
			{
				postfixDeque.push_back(opStack.top());
				opStack.pop();
			}

			if (!opStack.empty()) opStack.pop();
			else
			{
				std::cout << "ERROR @ Line " << line << ": Mismatched Parentheses in the expression." << std::endl;
				return std::deque<std::string>();
			}
		}

		else
		{
			if (!isVariableDefinedInAccessibleScopes(token, upperScopes))
			{
				std::cout << "ERROR @ Line " << line << ": Undefined variable used." << std::endl;
				return std::deque<std::string>();
			}

			else
			{
				if (getVariableType(token) != IntegerType)
				{
					std::cout << "ERROR @ Line " << line << ": Cannot do arithmetic on non-integral types." << std::endl;
					return std::deque<std::string>();
				}

				postfixDeque.push_back(token);
			}
		}
	}

	while (!opStack.empty())
	{
		if (opStack.top() == "(" || opStack.top() == ")")
		{
			std::cout << "ERROR @ Line " << line << ": Mismatched Parentheses in the expression." << std::endl;
			return std::deque<std::string>();
		}

		postfixDeque.push_back(opStack.top());
		opStack.pop();
	}

	return postfixDeque;
}

bool logical_IsOperator(const std::string& s)
{
	return (s == "AND" || s == "OR" || s == "NOT" || s == "<" || s == "<=" || s == ">" || s == ">=" || s == "==" || s == "!=");
}

bool logical_IsComparator(const std::string& s)
{
	return (s == "<" || s == "<=" || s == ">" || s == ">=" || s == "==" || s == "!=");
}

int logical_GetPrecedence(const std::string& s)
{
	if (s == "NOT") return 50;
	if (s == "<" || s == "<=" || s == ">" || s == ">=") return 40;
	if (s == "==" || s == "!=") return 30;
	if (s == "AND") return 20;
	if (s == "OR") return 10;
}

bool logical_IsLeftAssociative(const std::string& s)
{
	return !(s == "NOT");
}

std::vector<std::string> logical_ExpressionTokenizer(const std::string& expression)
{
	return regexTokenize(expression, std::regex("\\(|\\)|[^\\s()]+"));
}

std::deque<std::string> logical_InfixToPostfix(const std::string& expression, const int& line, const std::stack<std::string>& upperScopes)
{
	std::vector<std::string> tokens = logical_ExpressionTokenizer(expression);

	std::stack<std::string> opStack;
	std::deque<std::string> postfixDeque;

	// Parse Infix to Postfix
	for (const auto& token : tokens)
	{
		if (token == "TRUE" || token == "FALSE" || isNumericValue(token)) postfixDeque.push_back(token);

		else if (logical_IsOperator(token))
		{
			while (!opStack.empty() && ((logical_GetPrecedence(opStack.top()) > logical_GetPrecedence(token)) || ((logical_GetPrecedence(opStack.top()) == logical_GetPrecedence(token)) && logical_IsLeftAssociative(token)) && opStack.top() != "("))
			{
				postfixDeque.push_back(opStack.top());
				opStack.pop();
			}

			opStack.push(token);
		}

		else if (token == "(") opStack.push("(");

		else if (token == ")")
		{
			while (!opStack.empty() && opStack.top() != "(")
			{
				postfixDeque.push_back(opStack.top());
				opStack.pop();
			}

			if (!opStack.empty()) opStack.pop();
			else
			{
				std::cout << "ERROR @ Line " << line << ": Mismatched Parentheses in the expression." << std::endl;
				return std::deque<std::string>();
			}
		}

		else
		{
			if (!isVariableDefinedInAccessibleScopes(token, upperScopes))
			{
				std::cout << "ERROR @ Line " << line << ": Undefined/Inaccessible variable used => " << token << std::endl;
				return std::deque<std::string>();
			}

			else
			{
				if (getVariableType(token) != BooleanType && getVariableType(token) != IntegerType)
				{
					std::cout << "ERROR @ Line " << line << ": Cannot perform logical operations on given variable types." << std::endl;
					return std::deque<std::string>();
				}

				postfixDeque.push_back(token);
			}
		}
	}

	while (!opStack.empty())
	{
		if (opStack.top() == "(" || opStack.top() == ")")
		{
			std::cout << "ERROR @ Line " << line << ": Mismatched Parentheses in the expression." << std::endl;
			return std::deque<std::string>();
		}

		postfixDeque.push_back(opStack.top());
		opStack.pop();
	}

	return postfixDeque;
}

bool isNumericValue(const std::string& token)
{
	return (std::regex_match(token, std::regex("^[+-]?\\d+")));
}

std::string removeSpaces(const std::string& s)
{
	std::string final;

	for (const auto& c : s)
	{
		if (c != ' ') final += c;
	}

	return final;
}

// Allows regex to validate logical expression's correctness (bracket nesting is verified by infix to postfix)
std::string removeBrackets(const std::string& s)
{
	std::string final;

	for (const auto& c : s)
	{
		if (c == '(' || c == ')') continue;
		final += c;
	}

	return final;
}

// Allows regex to validate arithmetic expression's correctness (bracket nesting is verified by infix to postfix)
std::string removeSpacesAndBrackets(const std::string& s)
{
	std::string final;

	for (const auto& c : s)
	{
		if (c == ' ' || c == '(' || c == ')') continue;
		final += c;
	}

	return final;
}

std::vector<std::string> tokenize(const std::string& line)
{
	std::vector<std::string> tokens;
	bool inString = false;
	std::string current;

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

std::vector<std::string> regexTokenize(const std::string& line, const std::regex& format)
{
	std::vector<std::string> tokens;

	for (auto it = std::sregex_iterator(line.begin(), line.end(), format); it != std::sregex_iterator(); it++)
	{
		tokens.push_back((*it).str());
	}

	return tokens;
}

std::string escapeQuoteLiteral(const std::string& token)
{
	std::string final;

	final += '"';

	for (int i = 1; i < token.size() - 1; i++)
	{
		if (token[i] == '"') final += "\"\"";
		else final += token[i];
	}

	final += '"';

	return final;
}

std::string filenameWithoutExt(char* fileName)
{
	std::string withoutExt(fileName);
	return withoutExt.substr(0, withoutExt.find_last_of('.'));
}
