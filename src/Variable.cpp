#include "Variable.h"

Variable::Variable()
{
	(*this).type = CharacterType;
	(*this).scope = "GLOBAL";
}

Variable::Variable(DataTypes t, const std::string& s)
{
	(*this).type = t;
	(*this).scope = s;
}
