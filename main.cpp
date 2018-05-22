//编译器语法制导翻译和运行时储存分配部分由于时间仓促我只是对付了一个出来，而且而且解释执行
//后续我可能会增加功能可以在github上查看
//目前支持int float的加减乘除和打印，且不支持类型转换
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
//支持的类型char由于字面值比较麻烦没弄
enum v_type_n {
	float_,int_ , char_
};
//变量信息pos为距离栈顶偏移量types为类型
struct var_info
{
	size_t pos;
	v_type_n types;
};
//定义从类型到v_type_n的映射
template <class T>
v_type_n type_t = float_;


//分配栈
char *p = (char*)malloc(4096);
//栈顶指针不过由于没有实现函数功能所以和p是永远一样的
char *c = p;
//栈的起始分配位置最开头的8个字节当临时变量了
size_t c_pos=8;
//符号表
std::unordered_map<std::wstring, var_info> symbol_map;

int main()
{
	//初始化类型到类型枚举的映射
	type_t<int> = int_;
	type_t<float> = float_;
	type_t<char> = char_;
	//不要的词法符号
	null_symbol(space,L"\ ");
	null_symbol(newline,L"\n");
	null_symbol(newr,L"\r");
	null_symbol(newt,L"\t");
	//通过正则表达式定义终结符
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

	//下面是非终结符
	//数组还不支持
	symbol(array_);
	//不支持
	symbol(unre);
	//变量
	symbol(exp);
	//语句
	symbol(statement);
	//语句组
	symbol(statements);
	//数字本身是变量
	exp = { number }, [](std::wstring str) {
		//计算数字值和分配内存空间
		*(float*)(c + c_pos)=std::stod(str);
		var_info a;
		a.pos = c_pos;
		a.types=float_;
		c_pos += sizeof(int);
		return a;
	};
	exp = { int_number }, [](std::wstring str) {
		//计算数字值和分配内存空间（整形）
		*(int*)(c + c_pos) = std::stoi(str);
		var_info a;
		a.pos = c_pos;
		a.types = int_;
		c_pos += sizeof(int);
		return a;
	};
	exp = {id}, [](std::wstring str) {
		//从符号表中查找
		auto a = symbol_map.find(str);
		if (a == symbol_map.end()) {
			throw L"undefind id " + str;
		}
		else {
			return a->second;
		}
	};
	//声明语句
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