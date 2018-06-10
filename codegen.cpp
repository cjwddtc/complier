#include "codegen.h"
#include <algorithm>
#include <functional>
#include <assert.h>
#include <variant>
#include <iostream>
namespace codegen {
	codegen::function_::function_() :have_finish(true)
	{
	}

	bool function_::operator==(const function_ &o) const
	{
		return ret_type == o.ret_type && arg_type == o.arg_type && have_finish == have_finish;
	}

	std::wstring &type_info::t_type()
	{
		return std::get<std::wstring>(b_type);
	}
	function_type &type_info::f_type()
	{
		return std::get<function_type>(b_type);
	}
	void type_info::set_type(uint32_t t)
	{
		switch (t)
		{
		case 0:
			b_type = interger();
			break;
		case 1:
			b_type = number();
			break;
		case 2:
			b_type = function_type();
			break;
		}
	}
	uint32_t type_info::type_type()const
	{
		uint32_t flag=0;
		if (!plus.empty())
		{
			flag = plus.back() ? is_array : is_pointer;
		}
		return std::visit([](auto arg)->uint32_t {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, function_type>)
			{
				return is_function;
			}
			if constexpr (std::is_same_v<T, interger>)
			{
				if (arg.is_unsigned)
				{
					return is_unsigned | is_interger;
				}
				return is_interger;
			}
			if constexpr (std::is_same_v<T, number>)
			{
				return is_float;
			}
			return 0;
		}, b_type)|flag;
	}

	bool operator==(const function_type &a, const function_type &b)
	{
		return *a == *b;
	}

	bool type_info::operator==(const type_info &a) const {
		return b_type == a.b_type &&plus == a.plus;
	}

	void print(std::wostream &o, type_info &t)
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
			t.plus.pop_back();
			print(o, t);
			o << " *";
		}
	}
	name_space map;
	std::wostream &operator<<(std::wostream &o, type_info t)
	{
		print(o, t);
		return o;
	}

	std::wostream &operator<<(std::wostream &o, codegen::base_type &t)
	{
		std::visit([&o](auto arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, function_type>)
			{
				o << arg;
			}
			else {
				o << arg;
			}
		}, t);
		return o;
	}

	void print(std::wostream &o,codegen::function_type &t)
	{
		o << "(";
		bool flag = false;
		for (auto a : t->arg_type)
		{
			if (flag) {
				o << ",";
			}
			else {
				flag = true;
			}
			o << a;
		}
		if (!t->have_finish)
		{
			if (flag) {
				o << ",";
			}
			o << "...";
		}
		o << ")";
	}

	std::wostream &operator<<(std::wostream &o, codegen::function_type &t)
	{
		o << t->ret_type;
		print(o, t);
		return o;
	}
	std::wostream & operator<<(std::wostream & o, codegen::number & t)
	{
		switch (t.type)
		{
			case 0:
				o << "half";
				break;
			case 1:
				o << "float";
				break;
			case 2:
				o << "double";
				break;
			case 3:
				o << "fp128";
				break;
			default:
				assert(0);
		}
		return o;
	}
	std::wostream & operator<<(std::wostream & o, addr_var & t)
	{
		return o << t.type_ << "* " << t.real_name;

	}
	std::wostream & operator<<(std::wostream & o, tmp_var & t)
	{
		return o << t.type_ << " " << t.real_name;
	}
	void convert(tmp_var &s,const type_info &t, bool is_force)
	{
		if (t == s.type_)
		{
			return;
		}
		std::visit([](auto a,auto b) {
			using Ta = std::decay_t<decltype(a)>;
			using Tb = std::decay_t<decltype(b)>;
			if constexpr (std::is_same_v<T, function_type>)
			{
				return is_function;
			}
		},s.type_.b_type, t.b_type) ;
		else if (dt|is_interger && st|is_interger)
		{
			if(std::get<interger>(s.type_.b_type)>)
			auto a = cmp_type(s.type_.b_type, t.b_type);
			if (a == 0)
			{
				return s;
			}

			tmp_var v = map.newvar();
			std::wstring ins;
			if (a > 0)
			{
				ins = L"trunc";
			}
			else
			{
				ins = L"sext";
			}
			v.type_ = t;
			std::wcout << v.real_name << "=" << ins << " " << s.type_.t_type() << " " << s.real_name << " to " << t.t_type() << "\n";
			return v;
		}
		else if ((s.type_.is_void_ptr() && t.type_type() == pointer) || (t.is_void_ptr() && s.type_.type_type() == pointer)) {
			tmp_var v = map.newvar();
			v.type_ = t;
			std::wcout << v.real_name << "=bitcast " << s.type_ << " " << s.real_name << " to " << t << "\n";
			return v;
		}
		else if (is_force && t.type_type() == pointer && s.type_.type_type() == pointer) {
			tmp_var v = map.newvar();
			v.type_ = t;
			std::wcout << v.real_name << "=bitcast " << s.type_ << " " << s.real_name << " to " << t << "\n";
			return v;
		}
		else {
			throw "";
		}
	}

	addr_var &name_space::find(std::wstring str)
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
	addr_var &name_space::add(std::wstring name)
	{
		if (maps) {
			addr_var v;
			v.real_name = L"%" + name;
			return maps->second.emplace(name, v).first->second;
		}
		else {
			addr_var v;
			v.real_name = L"@" + name;
			return global.emplace(name, v).first->second;
		}
	}
	tmp_var name_space::newvar()
	{
		tmp_var v;
		if (maps) {
			v.real_name = L"%" + std::to_wstring(maps->first++);
		}
		else {
			v.real_name = L"@" + std::to_wstring(global_index++);
		}
		return v;

	}
	void name_space::delvar()
	{
		global_index--;
	}
	codegen::name_space::name_space() :global_index(1)
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
	addr_var tmp_var::deref()
	{
		addr_var v;
		v.real_name = real_name;
		v.type_ = type_;
		if (v.type_.type_type()!=pointer)
		{
			throw "you can not deference not pointer type";
		}
		v.type_.plus.pop_back();
		return v;
	}
	tmp_var tmp_var::ptr_off_set(tmp_var var)
	{
		if (type_.type_type() != pointer)
		{
			throw "you can not off_set not pointer type";
		}
		if (var.type_.type_type() != interger) {
			throw "index must be interger";
		}
		tmp_var v = map.newvar();
		v.type_ = type_;
		v.type_.plus.pop_back();
		std::wcout << v.real_name << " = " << "getelementptr " << v.type_ 
			<< ","	<< *this << "," << var << "\n";
		v.type_.plus.emplace_back(0);
		return v;
	}
	tmp_var addr_var::load()
	{
		tmp_var v = map.newvar();
		v.type_ = type_;
		std::wcout << v.real_name << "= load " << v.type_ << " , " << *this << "\n";
		return v;
	}

	void addr_var::save(tmp_var var)
	{
		std::wcout << "store " << var << " , " << *this << "\n";
	}
	tmp_var addr_var::addr()
	{
		tmp_var v;
		v.real_name = real_name;
		v.type_ = type_;
		v.type_.plus.push_back(0);
		return v;
	}
	tmp_var tmp_var::call(std::vector<tmp_var>& var)
	{
		tmp_var tv;
		if (type_.plus.size() == 1 && type_.plus.back() == 0 && type_.b_type.index() == 1)
		{
			auto f = type_.f_type();
			auto it = f->arg_type.begin();
			std::vector<tmp_var> v;
			v.reserve(var.size());
			std::transform(var.begin(), var.end(), std::back_inserter(v), [&it, f](tmp_var v) {
				if (it != f->arg_type.end())
					return convert(v, *it++, false);
				else if (f->have_finish)
					throw "to many function call";
				else
					return v;
			});
			if (it != f->arg_type.end()) {
				throw "no enough arg for the function";
			}
			if (!f->ret_type.is_void()) {
				tv = map.newvar();
				std::wcout << tv.real_name << " = ";
			}
			tv.type_ = f->ret_type;
			std::wcout << "call " << f->ret_type;
			print(std::wcout, f);
			std::wcout << " " << real_name << "(";
			bool flag = false;
			for (auto a : v)
			{
				if (flag)
				{
					std::wcout << ",";
				}
				else {
					flag = true;
				}
				std::wcout << a;
			}
			std::wcout << ")\n";
		}
		else {
			throw "you can not call a not function";
		}
		return tv;
	}
	void addr_var::func_sign(std::vector<std::wstring> names)
	{
		auto f = type_.f_type();
		std::wcout << "define " << f->ret_type << " " << real_name << "(";
		bool flag = false;
		for (auto a : f->arg_type)
		{
			if (flag)
			{
				std::wcout << ",";
			}
			else {
				flag = true;
			}
			std::wcout << a;
		}
		std::wcout << ")";
		auto it = names.begin();
		for (auto a : f->arg_type)
		{
			tmp_var tv = map.newvar();
			if (it->empty())
			{
				addr_var av = map.add(*it++);
				av.type_ = a;
				av.alloc();
				av.save(tv);
			}
		}
		return ;
	}
	void addr_var::func_sign()
	{
		auto f = type_.f_type();
		std::wcout << "declare " << f->ret_type << " " << real_name << "(";
		bool flag = false;
		for (auto a : f->arg_type)
		{
			if (flag)
			{
				std::wcout << ",";
			}
			else {
				flag = true;
			}
			std::wcout << a;
		}
		if (!f->have_finish)
		{
			if (flag)
			{
				std::wcout << ",";
			}
			std::wcout << "...";
		}
		std::wcout << ")";
		return;
	}
	void addr_var::alloc()
	{
		std::wcout << real_name << L"= alloca " << type_ << "\n";
	}
	addr_var addr_var::off_set(tmp_var index)
	{
		if (type_.type_type() == array) {
			return array_off_set(index);
		}
		else {
			return load().ptr_off_set(index).deref();
		}
		throw "you cannot off_set a" + type_.type_type();
		return addr_var();
	}
	addr_var addr_var::array_off_set(tmp_var index)
	{
		if (type_.type_type() != array)
		{
			throw "you can not off_set not pointer type";
		}
		if (index.type_.type_type() != interger) {
			throw "index must be interger";
		}
		tmp_var v = map.newvar();
		v.type_ = type_;
		std::wcout << v.real_name << " = " << "getelementptr " << v.type_
			<< "," << *this << ",i64 0," << index << "\n";
		v.type_.plus.back() = 0;
		return v.deref();
	}
	bool void_type::operator==(const void_type &) const
	{
		return true;
	}
}