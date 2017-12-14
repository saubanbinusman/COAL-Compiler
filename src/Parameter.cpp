#include "Parameter.h"

Parameter::Parameter(const std::string& n, const DataTypes& t, const bool& r)
{
	(*this).name = n;
	(*this).type = t;
	(*this).reference = r;
}