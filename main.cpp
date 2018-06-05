//�������﷨�Ƶ����������ʱ������䲿������ʱ��ִ���ֻ�ǶԸ���һ�����������Ҷ��ҽ���ִ��
//�����ҿ��ܻ����ӹ��ܿ�����github�ϲ鿴
//Ŀǰ֧��int float�ļӼ��˳��ʹ�ӡ���Ҳ�֧������ת��
#include "dfa.h"
#include "yufashu.hpp"
#include <iostream>
#include <iterator>
#include <fstream>
#include <variant>
#include "codegen.h"
using yacc::unit;
using yacc::unit_it;
using yacc::make_grammer;
using yacc::pass_by;
using yacc::not_use;

using namespace std::string_literals;
size_t tmp_id;
std::wostream &o = std::wcout;
codegen::name_space map;
using codegen::var;
using cg_type=codegen::type;
int main()
{
	//��Ҫ�Ĵʷ�����
	null_symbol(space,L"\ ");
	null_symbol(newline,L"\n");
	null_symbol(newr,L"\r");
	null_symbol(newt,L"\t");
	//ͨ��������ʽ�����ս��
	final_symbol(type, LR"(i8|i16|i32)");
	final_symbol(print, LR"(print)");
	final_symbol(number,LR"((1-9)(0-9)*(.(0-9)*(1-9))?)");
	final_symbol(int_number,LR"((1-9)(0-9)*)");
	final_symbol(id, LR"((a-z)+)");
	final_symbol(ar,LR"(\-\>)");
	final_symbol(op1,LR"(\+|\-)");
	final_symbol(op2, LR"(\/)");
	final_symbol(star_op2, LR"(\*)");
	final_symbol(bl, LR"(\()");
	final_symbol(br, LR"(\))");
	final_symbol(bigl, LR"(\{)");
	final_symbol(bigr, LR"(\})");

	final_symbol(ml,LR"(\[)");
	final_symbol(mr,LR"(\])");
	final_symbol(equal,LR"(\=)");
	final_symbol(sep, LR"(;)");
	final_symbol(comma, LR"(,)");
	final_symbol(dot, LR"(.)");

	//�����Ƿ��ս��
	//���黹��֧��
	symbol(array_);
	//��֧��
	symbol(unre);
	symbol(type_dec);
	symbol(dec);
	symbol(more_dec);
	//����
	symbol(exp);
	symbol(const_exp);
	//���
	symbol(statement);
	//�����
	symbol(statements);
	symbol(function_type);
	symbol(declare);
	symbol(root);
	symbol(block);
	symbol(arg_type_list);
	symbol(arg_list);
	symbol(bigl_);
	//���ֱ����Ǳ���
	exp = {id}, [](std::wstring str)->var {
		//�ӷ��ű��в���
		return map.find(str);
	};
	const_exp = { number }, [](std::wstring str)->var {
		//�ӷ��ű��в���
		var v;
		v.real_name = str;
		v.type.base_type.father()= L"i32";
		return v;
	};
	function_type = { type_dec,bl,arg_type_list ,br },
		[](cg_type ret_t, not_use, std::vector<cg_type> ts, not_use) {
		codegen::function f(new codegen::function_);
		f->arg_type = ts;
		f->ret_type = ret_t;
		return f;
	};
	type_dec = { type }, [](std::wstring str) {
		codegen::type t;
		t.base_type.father() = str;
		return t;
	};
	bigl_ = { bigl }, [](not_use) {
		map.enable(true);
		return not_use(); 
	};
	block = { function_type ,id,sep }, [](codegen::function f, std::wstring str) {
		var v = map.add(str);
		o << "declare " << f->to_string(v.real_name);
		v.type.base_type.father() = f;
		return v;
	};


	type_dec = { function_type }, [](codegen::function f) {
		cg_type t;
		t.base_type.father() = f;
		return t;
	};
	arg_type_list = { type_dec }, [](cg_type t) {
		std::vector<cg_type> ts;
		ts.emplace_back(t);
		return ts;
	};
	arg_type_list = {}, []() {
		std::vector<cg_type> ts;
		return ts;
	};
	arg_type_list = { arg_type_list ,comma,type_dec }, [](std::vector<cg_type> ts,not_use,cg_type t) {
		ts.emplace_back(t);
		return ts;
	};
	type_dec = { type_dec,ml,int_number,mr }, [](codegen::type t, not_use, std::wstring str, not_use) {
		t.plus.push_back(std::stoll(str));
		return t;
	};
	type_dec = { type_dec,star_op2 }, [](codegen::type t, not_use) {
		t.plus.push_back(0);
		return t;
	};
	auto dec_func= [](codegen::type i, std::wstring id) {
		var &v = map.add(id);
		v.type = i;
		o << v.real_name << L"= alloca " << (std::wstring)v.type << "\n";
		return i;
	};
	//�������
	more_dec = { type_dec,id }, dec_func;
	dec = { more_dec,comma,id }, dec_func;
	dec = { more_dec }, pass_by(0);
	declare = { dec,sep }, pass_by(0);
	statement = { declare }, pass_by(0);
	
	//���pass_by��ʾ��Լ�󣬱���Լ�ĵڶ�������backet��ֱֵ�Ӹ�����̳ɵ�backet
	exp = { bl,exp,br }, pass_by(1);
	exp = { bl,const_exp,br }, pass_by(1);
	const_exp = { exp }, [](var a) {
		var v = map.newvar();
		v.type = a.type;
		if (!a.type.is_variable()) {
			o << v.real_name << L"= load " << std::wstring(a.type) << L"," << std::wstring(a.type) << L"*" << a.real_name << "\n";
		}
		else {
			v.real_name = a.real_name;
		}
		return v;
	};
	const_exp ={ const_exp,op2,const_exp }, [](var a,std::wstring op,var b) {
		assert(a.type.plus.empty());
		assert(b.type.plus.empty());
		assert(a.type.base_type==b.type.base_type);
		std::wstring str;
		if (op == L"*") 
			str = L"mul";
		else if(op==L"/")
			str = L"div";
		var v= map.newvar();
		o << v.real_name << "=" << (a.type.base_type.is_interger() ? L"" : L"f")
			<< str << " " << std::wstring(a.type) << a.real_name << "," << b.real_name << L"\n";
		return v;
	};
	const_exp = { const_exp,op1,const_exp }, [](var a, std::wstring  op, var b) {
		assert(a.type == b.type);
		std::wstring str;
		if (op == L"+")
			str = L"add";
		else if (op == L"-")
			str = L"sub";
		var v = map.newvar();
		o << v.real_name << "=" << (a.type.base_type.is_interger() ? L"" : L"f")
			<< str << " " << std::wstring(a.type) << a.real_name << "," << b.real_name << L"\n";
		v.type = a.type;
		return v;
	};
	const_exp = { exp,equal,const_exp }, [](var a, not_use, var b) {
		assert(a.type == b.type);
		o << "store " << std::wstring(a.type) << " " << b.real_name << L"," << std::wstring(a.type) << "* " << a.real_name << "\n";
		return b;
	};
	root = { block },pass_by(0);
	root = { root,block }, pass_by(0);
	block = { statement }, pass_by(0);
	statement = { const_exp,sep }, pass_by(-1);
	//pass_by��-1��ʾ����������
	statements = { statement }, pass_by(-1);
	statements = { statements,statement }, pass_by(-1);
	auto a = make_dfa();
	bflag = 1;
	auto b = make_grammer(root, [](not_use a) {});
	//��ȡ��ִ��test.txt������
	std::wifstream ff("D:\\test.txt");
	auto it = a->read(std::istreambuf_iterator<wchar_t>(ff), std::istreambuf_iterator<wchar_t>());
	b->read(it.first, it.second);
	getchar();
	return 0;
}
