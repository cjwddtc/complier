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
			str += (std::wstring)*arg;
		}
	}, base_type.father());
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

codegen::function_::function_():have_finish(true)
{
}

std::wstring codegen::function_::to_function_dec(std::wstring name)
{
	std::wstring str(ret_type);
	str += L" ";
	str += name;
	str += L"(";
	bool flag = false;
	for (auto &a : arg_type)
	{
		if (flag) {
			str += L',';
		}
		else {
			flag = true;
		}
		str += a.first;
	}
	if (!have_finish) {
		str += L",...";
	}
	str += L')';
	return str;
}

std::wstring codegen::function_::to_function_call()
{
	std::wstring str(ret_type);
	str += L" ";
	str += L"(";
	bool flag = false;
	for (auto &a : arg_type)
	{
		if (flag) {
			str += L',';
		}
		else {
			flag = true;
		}
		str += a.first;
	}
	if (!have_finish) {
		str += L",...";
	}
	str += L')';
	return str;
}

bool type::is_variable()
{
	if (plus.empty()) {
		return base_type.index() == 0;
	}
	else {
		return plus.back() == 0;
	}
}

bool base_type::is_interger() {
	return true;
}
bool base_type::is_function() {
	return std::visit([](auto arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, function>)
		{
			return false;
		}
		else {
			return true;
		}
	}, father());
}
std::variant<std::wstring, function>&base_type::father()
{
	return (std::variant<std::wstring, function> &)*this;
}
bool base_type::operator==(const base_type & o)const
{
	if (index() == o.index())
	{
		if (index() == 1) {
			auto a = std::get<function>(o);
			auto b = std::get<function>(*this);
			bool f= a->ret_type == b->ret_type && a->arg_type == b->arg_type && a->have_finish == b->have_finish;
			return f;
		}
		else if (index() == 0)
		{
			return std::get<std::wstring>(o) == std::get<std::wstring>(*this);
		}
	}
	return false;
}
size_t type::size() {
	return 0;
}

bool type::operator==(const type &a) const{
	return base_type == a.base_type && plus == a.plus;
}
function_::operator std::wstring()
{
	std::wstring str(ret_type);
	str+= L"(";
	for (auto &a : arg_type)
	{
		str += a.first;
		str += L',';
	}
	if (have_finish) {
		str.pop_back();
	}
	else {
		str += L"...";
	}
	str += L")*";
	return str;
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