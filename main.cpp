//�������﷨�Ƶ����������ʱ������䲿������ʱ��ִ���ֻ�ǶԸ���һ�����������Ҷ��ҽ���ִ��
//�����ҿ��ܻ����ӹ��ܿ�����github�ϲ鿴
//Ŀǰ֧��int float�ļӼ��˳��ʹ�ӡ���Ҳ�֧������ת��
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
//֧�ֵ�����char��������ֵ�Ƚ��鷳ûŪ
enum v_type_n {
	i8,i16,i32 , i64,
	half_, float_, double_, fp128
};
//������ϢposΪ����ջ��ƫ����typesΪ����
struct var_info
{
	bool is_literal;
	std::wstring name;
	v_type_n base_type;
	std::vector<size_t> plus;
};

//���ű�

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
	bool add(std::wstring str, v_type_n base_type, std::vector<size_t> plus)
	{
		var_info v;
		v.base_type = base_type;
		v.is_literal = false;
		if (local_symbol_map.empty()) {

			v.name = L"@" + str;
			v.plus = plus;
			return local_symbol_map.front().emplace(str, std::move(v)).second;
		}
		else {
			v.name = L"%";
			std::fill_n(std::back_inserter(v.name), local_symbol_map.size() - 1, '.');
			v.name += str;
			v.plus = plus;
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
int main()
{
	//��Ҫ�Ĵʷ�����
	null_symbol(space,L"\ ");
	null_symbol(newline,L"\n");
	null_symbol(newr,L"\r");
	null_symbol(newt,L"\t");
	//ͨ��������ʽ�����ս��
	final_symbol(type, LR"(int|float|char)");
	final_symbol(print, LR"(print)");
	final_symbol(number,LR"((1-9)(0-9)*(.(0-9)*(1-9))?)");
	final_symbol(int_number,LR"((1-9)(0-9)*)");
	final_symbol(id, LR"((a-z)+)");
	final_symbol(ar,LR"(\-\>)");
	final_symbol(op1,LR"(\+|\-)");
	final_symbol(op2,LR"(\*|\/)");
	final_symbol(bl,LR"(\()");
	final_symbol(br,LR"(\))");

	final_symbol(ml,LR"(\[)");
	final_symbol(mr,LR"(\])");
	final_symbol(equal,LR"(\=)");
	final_symbol(sep,LR"(;)");

	//�����Ƿ��ս��
	//���黹��֧��
	symbol(array_);
	//��֧��
	symbol(unre);
	//����
	symbol(exp);
	//���
	symbol(statement);
	//�����
	symbol(statements);
	//���ֱ����Ǳ���
	exp = { number }, [](std::wstring str) {
		var_info v;
		v.base_type = double_;
		v.is_literal = true;
		v.name = str;
		return v;
	};
	exp = { int_number }, [](std::wstring str) {
		var_info v;
		v.base_type = i32;
		v.is_literal = true;
		v.name = str;
		return v;
	};
	exp = {id}, [](std::wstring str) {
		//�ӷ��ű��в���
		return map.find(str);
	};
	//�������
	statement = { type,id ,sep }, [](std::wstring str,std::wstring id) {

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
	//���pass_by��ʾ��Լ�󣬱���Լ�ĵڶ�������backet��ֱֵ�Ӹ�����̳ɵ�backet
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
	//��ӡ��䣬����not_use��ʾ������Ҫ����ķ����ŵ�����
	statement = { print,exp,sep }, [](not_use, var_info a) {
		print_all_type(a)
		return not_use();
	};
	//pass_by��-1��ʾ����������
	statement = { exp }, pass_by(-1);
	statements = { statement }, pass_by(-1);
	statements = { statements,statement }, pass_by(-1);
	auto a = make_dfa();
	bflag = 1;
	auto b = make_grammer(statements, [](not_use a) {});
	//��ȡ��ִ��test.txt������
	std::wifstream ff("D:\\test.txt");
	auto it = a->read(std::istreambuf_iterator<wchar_t>(ff), std::istreambuf_iterator<wchar_t>());
	b->read(it.first, it.second);
	getchar();
	return 0;
}
