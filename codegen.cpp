#include "codegen.h"
#include <algorithm>
#include <functional>
#include <assert.h>
#include <variant>
using namespace codegen;
type::operator std::wstring()
{
	std::wstring str;
	std::visit([&str](auto arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, std::wstring>)
		{
			str += arg;
		}
		if constexpr (std::is_same_v<T, function>)
		{
			str += (std::wstring)function;
		}
	}, this->base_type);
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

bool type::is_variable()
{
	return !(plus.empty() || plus.back() == 0);
}

size_t type::size() {
	return 0;
}



function::operator std::wstring()
{
	std::wstring str=L"(";
	for (auto &a : arg_type)
	{
		str += a;
		str += L',';
	}
	str.back() = L')';
	str += L"->";
	str += ret_type;
}

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
		v.real_name = L"%" + name;
		return maps->second.emplace( name, v).first->second;
	}
	else {
		var v;
		v.real_name = L"@" + name;
		return global.emplace(name, v).first->second;
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
codegen::name_space::name_space():global_index(1)
{
}
void name_space::enable(bool flag)
{
	if (flag) {
		maps.emplace();
		assert(maps.has_value());
		maps->first = 1;
	}
	else {
		maps.reset();
	}
}