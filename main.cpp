//编译器语法制导翻译和运行时储存分配部分由于时间仓促我只是对付了一个出来，而且而且解释执行
//后续我可能会增加功能可以在github上查看
//目前支持int float的加减乘除和打印，且不支持类型转换
#include "dfa.h"
#include "yufashu.hpp"
#include <iostream>
#include <iterator>
#include <fstream>
#include <variant>
using yacc::unit;
using yacc::unit_it;
using yacc::make_grammer;
using yacc::pass_by;
using yacc::not_use;
//支持的类型char由于字面值比较麻烦没弄
enum v_type_n {
	i8,i16,i32 , i64,
	half_, float_, double_, fp128_
};

struct type_i
{
	v_type_n base_type;
	std::vector<size_t> plus;
	operator std::wstring()
	{
		std::wstring str(ws_map[base_type]);
		for (auto a : plus)
		{
			if (a == 0) {
				str = str + L"*";
			}
			else {
				str = L"[ " + std::to_wstring(a) + L" * " + str + L" ]";
			}
		}
	}
};


struct tmp_var
{
	size_t i;
	type_i type;
};

struct address_var
{

};

//符号表

struct symbol_map
{
	std::unordered_map<std::wstring, var_info> global_symbol_map;
	std::list<std::unordered_map<std::wstring, var_info>> local_symbol_map;
	var_info &find(std::wstring str) {
		for (auto a : local_symbol_map)
		{
			auto it = a.find(str);
			if (it == a.end()) {
				return it->second;
			}
		}
		auto it = global_symbol_map.find(str);
		if (it != global_symbol_map.end())
		{
			return it->second;
		}
		throw L"undefind id " + str;
	}
	var_info &add(std::wstring str, v_type_n base_type, std::vector<size_t> plus)
	{
		var_info v;
		v.t_i.base_type = base_type;
		v.is_literal = false;
		if (local_symbol_map.empty()) {

			v.name = L"@" + str;
			v.t_i.plus = plus;
			auto it = local_symbol_map.front().emplace(str, std::move(v));
			if (!it.second) {
				throw L"redefine of " + v.name;
			}
			return.first;
		}
		else {
			v.name = L"%";
			std::fill_n(std::back_inserter(v.name), local_symbol_map.size() - 1, '.');
			v.name += str;
			v.t_i.plus = plus;
			return local_symbol_map.front().emplace(str, std::move(v)).second;
		}
	}
	bool is_global() {
		return local_symbol_map.empty();
	}
	void add()
	{
		local_symbol_map.emplace_front();
	}
	void remove()
	{
		local_symbol_map.pop_front();
	}
};
symbol_map map;
using namespace std::string_literals;
size_t tmp_id;
std::wostream &o = std::wcout;
std::unordered_map<std::wstring, v_type_n> type_map;
std::wstring_view ws_map[8] = { L"int8",L"int16" ,L"int32" ,L"int64",L"half",L"float",L"double",L"ldouble"};
int main()
{
	for (auto i=0;i!=8;i++)
	{
		type_map[std::wstring(ws_map[i])] = (v_type_n)i;
	}
	//不要的词法符号
	null_symbol(space,L"\ ");
	null_symbol(newline,L"\n");
	null_symbol(newr,L"\r");
	null_symbol(newt,L"\t");
	//通过正则表达式定义终结符
	final_symbol(type, LR"(int|float|char)");
	final_symbol(print, LR"(print)");
	final_symbol(number,LR"((1-9)(0-9)*(.(0-9)*(1-9))?)");
	final_symbol(int_number,LR"((1-9)(0-9)*)");
	final_symbol(id, LR"((a-z)+)");
	final_symbol(ar,LR"(\-\>)");
	final_symbol(op1,LR"(\+|\-)");
	final_symbol(op2, LR"(\/)");
	final_symbol(star_op2, LR"(\*)");
	final_symbol(bl,LR"(\()");
	final_symbol(br,LR"(\))");

	final_symbol(ml,LR"(\[)");
	final_symbol(mr,LR"(\])");
	final_symbol(equal,LR"(\=)");
	final_symbol(sep,LR"(;)");

	//下面是非终结符
	//数组还不支持
	symbol(array_);
	//不支持
	symbol(unre);
	symbol(type_dec);
	//变量
	symbol(exp);
	//语句
	symbol(statement);
	//语句组
	symbol(statements);
	//数字本身是变量
	exp = { number }, [](std::wstring str) {
		var_info v;
		v.t_i.base_type = double_;
		v.is_literal = true;
		v.name = str;
		return v;
	};
	exp = { int_number }, [](std::wstring str) {
		var_info v;
		v.t_i.base_type = i32;
		v.is_literal = true;
		v.name = str;
		return v;
	};
	exp = {id}, [](std::wstring str) {
		//从符号表中查找
		return map.find(str);
	};
	type_dec = { type }, [](std::wstring str) {
		type_i t;
		t.base_type = type_map[str];
		return t;
	};
	type_dec = { type_dec,ml,int_number,mr }, [](type_i t, not_use, std::wstring str, not_use) {
		t.plus.push_back(std::stoll(str));
		return t;
	};
	type_dec = { type_dec,star_op2 }, [](type_i t, not_use) {
		t.plus.push_back(0);
		return t;
	};
	//声明语句
	statement = { type_dec,id ,sep }, [](type_i i,std::wstring id) {
		if (map.is_global()) {
			o << "@" << id << "= global " << (std::wstring)i;
			map.add(id, i.base_type, i.plus);
		}
		else {
			map.add(id, i.base_type, i.plus);
		}
	};
	//这个pass_by表示规约后，被规约的第二个符号backet的值直接赋给规程成的backet
	exp = { bl,exp,br }, pass_by(1);
#define __var(type,b) (*(type*)(c+b.pos))

#define type_op(type,op,a,b) \
if(a.types==type_t<type>){\
	(*(type*)(c))=__var(type,a) op __var(type,b);\
}
#define op_run(op,a,b) \
	type_op(float, op, a, b)\
	else type_op(int, op, a, b)\
	else type_op(char, op, a, b)\

	exp ={ exp,op2,exp }, [](var_info a,std::wstring op,var_info b) {
		if (a.types == b.types) {
			var_info t;
			t.pos = 0;
			switch (op[0])
			{
			case L'*':
				op_run(*,a,b)
				break;
			case L'/':
				op_run(/, a, b)
				break;
			}
			t.types = a.types;
			return t;
		}
		else {
			throw L"same type can operate";
		}
	};
	exp = { exp,op1,exp }, [](var_info a, std::wstring  op, var_info b) {
		if (a.types == b.types) {
			var_info t;
			t.pos = 0;
			switch (op[0])
			{
			case L'+':
				op_run(+, a, b)
					break;
			case L'=':
				op_run(- , a, b)
					break;
			}
			t.types = a.types;
			return t;
		}
		else {
			throw L"same type can operate";
		}
	};
	exp = { exp,equal,exp }, [](var_info a, not_use, var_info b) {
		if (a.types == b.types) {
			var_info t;
			t.pos = 0;
			t.types = a.types;
			op_run(=, a, b)
			return t;
		}
		else {
			throw L"same type can operate";
		}
	};
	statement = { exp,sep }, pass_by(-1);
#define print_type(type,f,a) \
if(a.types==type_t<type>){\
	printf("%"#f"\n",__var(type,a));\
}
#define print_all_type(a) \
print_type(float,f,a)\
else print_type(int,d,a)\
else print_type(char,c,a)
	//打印语句，这里not_use表示并不需要这个文法符号的数据
	statement = { print,exp,sep }, [](not_use, var_info a) {
		print_all_type(a)
		return not_use();
	};
	//pass_by传-1表示不生成数据
	statement = { exp }, pass_by(-1);
	statements = { statement }, pass_by(-1);
	statements = { statements,statement }, pass_by(-1);
	auto a = make_dfa();
	bflag = 1;
	auto b = make_grammer(statements, [](not_use a) {});
	//读取并执行test.txt的内容
	std::wifstream ff("D:\\test.txt");
	auto it = a->read(std::istreambuf_iterator<wchar_t>(ff), std::istreambuf_iterator<wchar_t>());
	b->read(it.first, it.second);
	getchar();
	return 0;
}
