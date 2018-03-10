#include "dfa.h"
#include "yufashu.hpp"
#include <iostream>
#include <iterator>
#include <fstream>
using yacc::unit;
using yacc::unit_it;
using yacc::make_grammer;
using yacc::pass_by;
using yacc::not_use;
enum v_type_n {
	float_,int_ , char_
};
struct var_info
{
	size_t pos;
	v_type_n types;
};
template <class T>
v_type_n type_t = float_;



char *p = (char*)malloc(4096);
char *c = p;
size_t c_pos=8;
std::unordered_map<std::wstring, var_info> symbol_map;
int main()
{
	type_t<int> = int_;
	type_t<float> = float_;
	type_t<char> = char_;
	final_symbol type(LR"((int)|(float)|(char))");
	final_symbol id(LR"((a-b)+)");
	final_symbol print(LR"(print)");
	final_symbol number(LR"((1-9)(0-9)*(.(0-9)*(1-9))?)");
	final_symbol ar(LR"(\-\>)");
	final_symbol op1(LR"(\+|\-)");
	final_symbol op2(LR"(\*|\/)");
	final_symbol bl(LR"(\()");
	final_symbol br(LR"(\))");
	final_symbol ml(LR"(\[)");
	final_symbol mr(LR"(\])");
	final_symbol equal(LR"(\=)");
	final_symbol sep(LR"(;)");
	final_symbol newline(LR"(\
)");
	symbol array_;
	symbol unre;
	symbol muti;
	symbol backet;
	symbol plus;
	symbol assign;
	symbol var;
	symbol declartor;
	symbol statement;
	symbol statements;
	var = { number }, [](std::wstring str) {
		*(int*)(c + c_pos)=std::stod(str);
		var_info a;
		a.pos = c_pos;
		a.types=int_;
		c_pos += sizeof(int);
		return a;
	};
	var = {id}, [](std::wstring str) {
		auto a = symbol_map.find(str);
		if (a == symbol_map.end()) {
			throw L"undefind id " + str;
		}
		else {
			return a->second;
		}
	};
	declartor = { type,id ,sep }, [](std::wstring str,std::wstring id) {
		auto a = symbol_map.find(str);
		if (a != symbol_map.end()) {
			throw L"redeclare id " + str;
		}
		auto &b = symbol_map[id];
		b.pos = c_pos;
		if (str == L"int")
		{
			b.types = int_;
			c_pos += sizeof(int);
		}
		if (str == L"float")
		{
			b.types = float_;
			c_pos += sizeof(float);
		}
		if (str == L"char")
		{
			b.types = char_;
			c_pos += sizeof(char);
		}
		return b;
	};

	backet = { bl,backet,br }, pass_by(1);
	backet = { var }, pass_by(0);
#define __var(a,b) (*(a*)(c+b.pos))

#define type_op(type,op,a,b) \
if(a.types=type_t<type>){\
	(*(type*)(c))=__var(type,a) op __var(type,b);\
}
#define op_run(op,a,b) \
	type_op(float, op, a, b)\
	else type_op(int, op, a, b)\
	else type_op(char, op, a, b)\

	muti = { backet }, pass_by(0);
	muti={ muti,op2,muti }, [](var_info a,std::wstring op,var_info b) {
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
	plus ={muti}, pass_by(0);
	plus = { plus,op1,plus }, [](var_info a, std::wstring  op, var_info b) {
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
	assign = { plus }, pass_by(0);
	assign = { var,equal,plus,sep }, [](var_info a, not_use, var_info b) {
		if (a.types == b.types) {
			var_info t;
			t.pos = 0;
			op_run(=, a, b)
			return t;
		}
		else {
			throw L"same type can operate";
		}
	};
	statement = { assign }, pass_by(-1);
#define print_type(type,f,a) \
if(a.types=type_t<type>){\
	printf(#f##"\n",__var(type,a));\
}
#define print_all_type(a) \
print_type(float,f,a)\
else print_type(int,d,a)\
else print_type(char,c,a)

	statement = { print,plus,sep }, [](not_use, var_info a) {
		print_all_type(a)
		return not_use();
	};
	statement = { declartor }, pass_by(-1);
	statements = { statement }, pass_by(-1);
	statements = { statements,statement }, pass_by(-1);
	auto a = make_dfa();
	auto b = make_grammer(statements, [](not_use a) {return not_use(); });
	std::wifstream ff("D:\\test.txt");
	auto it = a->read(std::istreambuf_iterator<wchar_t>(std::wcin), std::istreambuf_iterator<wchar_t>());
	b->read(it.first, it.second);
	return 0;
}
