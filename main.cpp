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

using namespace std::string_literals;
size_t tmp_id;
std::wostream &o = std::wcout;
std::unordered_map<std::wstring, std::wstring> type_map;
std::wstring_view ir_map[8] = { L"i8",L"i16",L"i32",L"i64",L"half",L"float",L"double",L"f128" };
std::wstring_view lu_map[8] = { L"int8",L"int16" ,L"int32" ,L"int64",L"half",L"float",L"double",L"ldouble" };

struct type_i
{
	std::wstring base_type;
	std::vector<size_t> plus;
	operator std::wstring()
	{
		std::wstring str(type_map[base_type]);
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
	std::wstring real_name;
	type_i type;
};

struct address_var
{
	std::wstring real_name;
	type_i type;
};

std::wostream& operator <<(std::wostream &o, tmp_var &v){}
//���ű�

struct symbol_map
{
	std::unordered_map<std::wstring, address_var> global_symbol_map;
	std::list<std::unordered_map<std::wstring, address_var>> local_symbol_map;
	size_t tmp_id;
	size_t new_tmp();
	address_var &find(std::wstring str) {
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
	address_var &add(std::wstring str, type_i t)
	{
	}
	bool is_global() {
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
int main()
{
	for (int i = 0; i < 8; i++) {
		type_map.emplace(std::wstring(ir_map[i]), std::wstring(lu_map[i]));
		type_map.emplace(std::wstring(lu_map[i]), std::wstring(ir_map[i]));
	}
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
	final_symbol(op2, LR"(\/)");
	final_symbol(star_op2, LR"(\*)");
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
	symbol(type_dec);
	//����
	symbol(exp);
	//���
	symbol(statement);
	//�����
	symbol(statements);
	//���ֱ����Ǳ���
	exp = { number }, [](std::wstring str) {
		tmp_var v;
		v.i=map.new_tmp();
		v.type.base_type = L"float";
		o << v << "= fadd f128 0.0 ,f128 " << str;
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
		//�ӷ��ű��в���
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
	//�������
	statement = { type_dec,id ,sep }, [](type_i i,std::wstring id) {
		if (map.is_global()) {
			o << "@" << id << "= global " << (std::wstring)i;
			map.add(id, i.base_type, i.plus);
		}
		else {
			map.add(id, i.base_type, i.plus);
		}
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
