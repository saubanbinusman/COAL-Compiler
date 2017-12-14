#include "Variable.h"

Variable::Variable(DataTypes t, const std::string& s)
{
	(*this).type = t;
	(*this).scope = s;
	(*this).reference = false;
}

Variable::Variable(DataTypes t, const std::string& s, const bool& r)
{
	(*this).type = t;
	(*this).scope = s;
	(*this).reference = r;
}
