//�������﷨�Ƶ����������ʱ������䲿������ʱ��ִ���ֻ�ǶԸ���һ�����������Ҷ��ҽ���ִ��
//�����ҿ��ܻ����ӹ��ܿ�����github�ϲ鿴
//Ŀǰ֧��int float�ļӼ��˳��ʹ�ӡ���Ҳ�֧������ת��
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
//֧�ֵ�����char��������ֵ�Ƚ��鷳ûŪ
enum v_type_n {
	float_,int_ , char_
};
//������ϢposΪ����ջ��ƫ����typesΪ����
struct var_info
{
	size_t pos;
	v_type_n types;
};
//��������͵�v_type_n��ӳ��
template <class T>
v_type_n type_t = float_;


//����ջ
char *p = (char*)malloc(4096);
//ջ��ָ�벻������û��ʵ�ֺ����������Ժ�p����Զһ����
char *c = p;
//ջ����ʼ����λ���ͷ��8���ֽڵ���ʱ������
size_t c_pos=8;
//���ű�
std::unordered_map<std::wstring, var_info> symbol_map;

int main()
{
	//��ʼ�����͵�����ö�ٵ�ӳ��
	type_t<int> = int_;
	type_t<float> = float_;
	type_t<char> = char_;
	//��Ҫ�Ĵʷ�����
	null_symbol(space,L"\ ");
	null_symbol(newline,L"\n");
	null_symbol(newr,L"\r");
	null_symbol(newt,L"\t");
	//ͨ��������ʽ�����ս��
	final_symbol(id, LR"((a-z)+)");
	final_symbol(type,LR"(int|float|char)");
	final_symbol(print,LR"(print)");
	final_symbol(number,LR"((1-9)(0-9)*(.(0-9)*(1-9))?)");
	final_symbol(int_number,LR"((1-9)(0-9)*)");
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
		//��������ֵ�ͷ����ڴ�ռ�
		*(float*)(c + c_pos)=std::stod(str);
		var_info a;
		a.pos = c_pos;
		a.types=float_;
		c_pos += sizeof(int);
		return a;
	};
	exp = { int_number }, [](std::wstring str) {
		//��������ֵ�ͷ����ڴ�ռ䣨���Σ�
		*(int*)(c + c_pos) = std::stoi(str);
		var_info a;
		a.pos = c_pos;
		a.types = int_;
		c_pos += sizeof(int);
		return a;
	};
	exp = {id}, [](std::wstring str) {
		//�ӷ��ű��в���
		auto a = symbol_map.find(str);
		if (a == symbol_map.end()) {
			throw L"undefind id " + str;
		}
		else {
			return a->second;
		}
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

int main_() {

	final_symbol(number, LR"((1-9)(0-9)*(.(0-9)*(1-9))?)");
	final_symbol(sep, LR"(;)");
	auto a = make_dfa();
	std::vector<char> vec;
	vec.push_back('5');
	vec.push_back('5');
	vec.push_back('.');
	vec.push_back('4');
	vec.push_back(';');
	a->read(vec.begin(), vec.end());
	return 0;
}