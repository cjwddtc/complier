#include "codegen.h"
#include <algorithm>
#include <functional>
using namespace codegen;
type::operator std::wstring()
{
	std::wstring str=base_type;
	for (auto a : plus) {
		if (a) {
			str = L"[" + std::to_wstring(a) + L" x " + str + L"]";
		}
		else {
			str += L'*';
		}
	}
	return str;
}

bool type::is_array()
{
	return !(plus.empty() || plus.back() == 0);
}

size_t type::size() {
	return 0;
}

using namespace std::placeholders;

var &name_space::find(std::wstring str)
{
	if (maps) {
		auto it = maps->second.find(str);
		if (it != maps->second.end())
		{
			return it->second;
		}
	}
	auto it = global.find(str);
	if (it != global.end())
	{
		return it->second;
	}
	throw str;
}
var &name_space::add(std::wstring name)
{
	if (maps) {
		var v;
		v.real_name = L"@" + name;
		return maps->second.emplace( name, v).first->second;
	}
}
var name_space::newvar()
{
	var v;
	if (maps) {
		v.real_name = L"%"+std::to_wstring(maps->first++);
	}
	else {
		v.real_name = L"@"+std::to_wstring(global_index++);
	}
	return  v;

}
void name_space::enable(bool flag)
{
	if (flag) {
		maps.emplace();
	}
	else {
		maps.reset();
	}
}