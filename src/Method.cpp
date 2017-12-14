#include "Method.h"

Method::Method(const std::string& n, const std::vector<Parameter>& p, const DataTypes& r, const bool& ret)
{
	name = n;
	params = p;
	returnType = r;
	returned = ret;
}