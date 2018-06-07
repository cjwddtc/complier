#include "codegen.h"
#include <algorithm>
#include <functional>
#include <assert.h>
#include <variant>
#include <iostream>
using namespace codegen;
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

std::wstring &type::t_type()
{
	return std::get<std::wstring>(b_type);
}
function_type type::f_type()
{
	return std::get<function_type>(b_type);
}
value_type codegen::type::type_type()const
{
	if (plus.empty())
		return std::visit([](auto arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, function_type>)
		{
			return function;
		}
		else {
			if (arg[0] == L'i')
				return interger;
			else
				return number;
		}
	}, b_type);
	else if (plus.back())
		return array;
	else
		return pointer;
}

bool type::operator==(const type &a) const{
	if (b_type.index() == a.b_type.index() && plus==a.plus) 
		if (b_type.index() == 0)
			return b_type == a.b_type;
		else
			return *std::get<function_>(b_type) == *std::get<function_>(a.b_type);
	return false;
}


int codegen::cmp_type(base_type a, base_type b)
{
	return std::stoi(std::get<std::wstring>(a).c_str() + 1) - std::stoi(std::get<std::wstring>(b).c_str() + 1);
}
bool type::is_void_ptr()
{
	return plus.size() == 1 && plus.back()==0 && t_type()==L"void";
}

void print(std::wostream &o, type &t)
{
	if (t.plus.empty())
	{
		o << t.b_type;
	}
	else if (t.plus.back())
	{
		o << "[ " << t.plus.back() << " x ";
		t.plus.pop_back();
		print(o, t);
		o << " ]";
	}
	else
	{
		print(o, t);
		o << " *";
	}
}
name_space codegen::map;
std::wostream &operator<<(std::wostream &o, type t)
{
	print(o, t);
}
tmp_var convert(tmp_var s, type t, bool is_force)
{

	auto dt=t.type_type();
	auto st = s.type_.type_type();
	if (dt == interger && st == interger)
	{
		auto a = cmp_type(s.type_.b_type, t.b_type);
		if (a == 0)
		{
			return s;
		}

		tmp_var v = map.newvar();
		std::wstring ins;
		if (a > 0)
		{
			ins=  L"trunc";
		}
		else
		{
			ins  = L"sext";
		}
		std::wcout << v.real_name << "=" << ins << " " << s.type_.t_type() << " " << s.real_name << " to " << t.t_type() << "\n";
		return v;
	}
	else if((s.type_.is_void_ptr() &&t.type_type()==pointer) || (t.is_void_ptr() && s.type_.type_type() == pointer)){
		tmp_var v = map.newvar();
		v.type_ = t;
		std::wcout << v.real_name << "=bitcast " << s.type_ << " " << s.real_name << " " << t << "\n";
		return v;
	}
	else if (is_force) {
		tmp_var v = map.newvar();
		v.type_ = t;
		std::wcout << v.real_name << "=bitcast " << s.type_ << " " << s.real_name << " " << t << "\n";
		return v;
	}
	else {
		throw "";
	}
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