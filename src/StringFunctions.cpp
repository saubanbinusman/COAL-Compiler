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

bool isOperator(const std::string& s)
{
	return (s == "+" || s == "-" || s == "*" || s == "/" || s == "%" || s == "^" || s == "__p" || s == "__m");
}

int getPrecedence(const std::string& s)
{
	if (s == "^" || s == "__p" || s == "__m") return 30;
	if (s == "*" || s == "/" || s == "%") return 20;
	if (s == "+" || s == "-") return 10;
}

bool isLeftAssociative(const std::string& s)
{
	return !(s == "^" || s == "m" || s == "p");
}

bool isNumericValue(const std::string& token)
{
	for (const auto& c : token)
	{
		if (!isdigit(c)) return false;
	}

	return true;
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

std::vector<std::string> expressionTokenizer(const std::string& expression)
{
	std::vector<std::string> tokens;
	std::string currentToken;

	for (int i = 0; i < expression.size(); i++)
	{
		if (i == expression.size() - 1)
		{
			if (isOperator(std::string(1, expression[i])) || expression[i] == '(' || expression[i] == ')')
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

		else if (isOperator(std::string(1, expression[i])) || expression[i] == '(' || expression[i] == ')')
		{
			if (!currentToken.empty()) tokens.push_back(currentToken);
			currentToken.clear();

			if (expression[i] == '+' || expression[i] == '-')
			{
				// Checking if it's a unary + or -
				if (i != 0 && (isOperator(std::string(1, expression[i - 1])) || expression[i - 1] == '('))
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

std::deque<std::string> infixToPostfix(const std::string& expression, const int& line)
{
	std::string trimmedExpr = removeSpaces(expression);
	std::vector<std::string> tokens = expressionTokenizer(trimmedExpr);

	std::stack<std::string> opStack;
	std::deque<std::string> postfixDeque;

	// Parse Infix to Postfix
	for (const auto& token : tokens)
	{
		if (isNumericValue(token)) postfixDeque.push_back(token);

		else if (isOperator(token))
		{
			while (!opStack.empty() && ((getPrecedence(opStack.top()) > getPrecedence(token)) || ((getPrecedence(opStack.top()) == getPrecedence(token)) && isLeftAssociative(token)) && opStack.top() != "("))
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
				std::cout << "ERROR @ Line " << line << ": Mismatched Parantheses in the expression." << std::endl;
				return std::deque<std::string>();
			}
		}

		else
		{
			if (!isVariableDefined(token))
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

	if (!opStack.empty())
	{
		if (opStack.top() == "(" || opStack.top() == ")")
		{
			std::cout << "ERROR @ Line " << line << ": Mismatched Parantheses in the expression." << std::endl;
			return std::deque<std::string>();
		}

		postfixDeque.push_back(opStack.top());
		opStack.pop();
	}

	return postfixDeque;
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
