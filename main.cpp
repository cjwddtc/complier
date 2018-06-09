//编译器语法制导翻译和运行时储存分配部分由于时间仓促我只是对付了一个出来，而且而且解释执行
//后续我可能会增加功能可以在github上查看
//目前支持int float的加减乘除和打印，且不支持类型转换
#include "dfa.h"
#include "yufashu.hpp"
#include <iostream>
#include <iterator>
#include <fstream>
#include <variant>
#include <algorithm>
#include "codegen.h"
using yacc::unit;
using yacc::unit_it;
using yacc::make_grammer;
using yacc::pass_by;
using yacc::not_use;

using namespace std::string_literals;
size_t tmp_id;
using codegen::map;
using codegen::tmp_var;
using codegen::addr_var;
using cg_type=codegen::type_info;

int main(int num,char **arg_ar)
{
	//不要的词法符号
	null_symbol(space,L"\ ");
	null_symbol(newline,L"\n");
	null_symbol(newr,L"\r");
	null_symbol(newt,L"\t");
	//通过正则表达式定义终结符
	final_symbol(type, LR"(i8|i16|i32|void)");
	final_symbol(ret, LR"(return)");
	final_symbol(print, LR"(print)");
	final_symbol(int_number,LR"(0|(1-9)(0-9)*)");
	final_symbol(id, LR"((a-z|_)+)");
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
	final_symbol(more, LR"(...)");
	final_symbol(addr, LR"(&)");

	//下面是非终结符
	//数组还不支持
	symbol(array_);
	//不支持
	symbol(unre);
	symbol(type_dec);
	symbol(dec);
	symbol(more_dec);
	//变量
	symbol(exp);
	symbol(const_exp);
	//语句
	symbol(statement);
	//语句组
	symbol(statements);
	symbol(function_type);
	symbol(function);
	symbol(function_prefix);
	symbol(root);
	symbol(declare);
	symbol(block);
	symbol(arg_id_list);
	symbol(arg_ids);
	symbol(args);
	symbol(arg_list);
	symbol(arg_node);
	exp = {id}, [](std::wstring str)->addr_var {
		//从符号表中查找
		return map.find(str);
	};
	//数字本身是变量
	const_exp = { int_number }, [](std::wstring str)->tmp_var {
		tmp_var v;
		v.real_name = str;
		v.type_.t_type() = L"i32";
		return v;
	};
	function_type = { type_dec,bl,arg_list ,br },
		[](cg_type ret_t, not_use,std::pair<codegen::function_type,std::vector<std::wstring> > ts, not_use) {
		ts.first->ret_type = ret_t;
		return ts.first;
	};
	type_dec = { type }, [](std::wstring str) {
		cg_type t;
		t.t_type() = str;
		return t;
	};
	block = { type_dec,id,bl,arg_list,br,sep }, [](cg_type f, std::wstring str,not_use, std::pair<codegen::function_type, std::vector<std::wstring> > ts) {

		addr_var &v = map.add(str);
		v.type_.set_type(true);
		ts.first->ret_type = f;
		v.type_.f_type() = ts.first;
		v.func_sign();
		std::cout << "\n";
		return 0;
	};

	function_prefix = { type_dec,id,bl,arg_list,br,bigl }, [](cg_type f, std::wstring str, not_use, std::pair<codegen::function_type, std::vector<std::wstring> > ts) {

		addr_var &v = map.add(str);
		v.type_.set_type(true);
		ts.first->ret_type = f;
		v.type_.f_type() = ts.first;
		v.func_sign(ts.second);
		map.enable(true);
		std::cout << "\n{";
		return 0;
	};
	block = { function_prefix,statements,bigr }, []() {
		map.enable(false);
		std::wcout << "}\n";
		return 0;
	};
	type_dec = { function_type,star_op2 }, [](codegen::function_type f) {
		cg_type t;
		t.set_type(true);
		t.f_type() = f;
		t.plus.push_back(0);
		return t;
	};
	arg_node = { type_dec }, [](cg_type t) {
		std::pair<cg_type, std::wstring> ts(t,L"");
		return ts;
	};
	arg_node = { type_dec,id }, [](cg_type t,std::wstring str) {
		std::pair<cg_type, std::wstring> ts(t, str);
		return ts;
	};
	arg_list = { args }, pass_by(0);
	arg_list = {}, []() {
		codegen::function_type f(new codegen::function_);
		std::pair<codegen::function_type, std::vector<std::wstring> > p;
		p.first = f;
		return p; 
	};
	args = { arg_node }, [](std::pair<cg_type, std::wstring> t) {
		codegen::function_type f(new codegen::function_);
		f->arg_type.emplace_back(t.first);
		std::pair<codegen::function_type, std::vector<std::wstring> > p;
		p.first = f;
		p.second.emplace_back(t.second);
		return p;
	};
	args = { args ,comma,arg_node }, [](std::pair<codegen::function_type, std::vector<std::wstring> > ts,not_use, std::pair<cg_type, std::wstring> t) {
		ts.first->arg_type.emplace_back(t.first);
		ts.second.emplace_back(t.second);
		return ts;
	};
	args = { args,comma,more }, [](std::pair<codegen::function_type, std::vector<std::wstring> > f) {
		f.first->have_finish = false;
		return f; 
	};
	arg_ids = { const_exp }, [](tmp_var t) {
		std::vector<tmp_var> vs;
		vs.emplace_back(t);
		return vs;
	};
	arg_ids = { arg_ids ,comma,const_exp }, [](std::vector<tmp_var> ts, not_use, tmp_var t) {
		ts.emplace_back(t);
		return ts;
	};
	arg_id_list = { arg_ids }, pass_by(0);
	arg_id_list = {}, []() {
		std::vector<tmp_var> vs;
		return vs;
	};
	const_exp = { exp,bl,arg_id_list,br }, [](addr_var v,not_use, std::vector<tmp_var> ts) {
		return v.call(ts);
	};
	type_dec = { type_dec,ml,int_number,mr }, [](codegen::type_info t, not_use, std::wstring str, not_use) {
		t.plus.push_back(std::stoll(str));
		return t;
	};
	type_dec = { type_dec,star_op2 }, [](codegen::type_info t, not_use) {
		t.plus.push_back(0);
		return t;
	};
	auto dec_func= [](codegen::type_info i, std::wstring id) {
		addr_var &v = map.add(id);
		v.type_ = i;
		v.alloc();
		return i;
	};
	//声明语句
	more_dec = { type_dec,id }, dec_func;
	dec = { more_dec,comma,id }, dec_func;
	dec = { more_dec }, pass_by(0);
	declare = { dec,sep }, pass_by(0);
	statement = { declare }, pass_by(0);
	statement = { ret,const_exp,sep }, [](not_use,tmp_var a) {
		std::wcout << "ret " << a << "\n";
		return 0;
	};
	//这个pass_by表示规约后，被规约的第二个符号backet的值直接赋给规程成的backet
	exp = { bl,exp,br }, pass_by(1);
	exp = { bl,const_exp,br }, pass_by(1);
	exp = { exp ,ml,const_exp,mr }, [](addr_var a, not_use, tmp_var b) {
		return a.off_set(b);
	};
	exp = {const_exp ,ml,const_exp,mr }, [](tmp_var a, not_use, tmp_var b) {
		a.ptr_off_set(b).deref();
	};
	const_exp = { exp }, [](addr_var a) {
		return a.load();
	};
	const_exp = { addr ,exp }, [](not_use, addr_var v)
	{
		return v.addr();
	};
	op2 = { star_op2 }, pass_by(0);
	const_exp ={ const_exp,op2,const_exp }, [](tmp_var a,std::wstring op, tmp_var b) {
		tmp_var v = map.newvar();
		if (a.type_.type_type() == codegen::interger && b.type_.type_type() == codegen::interger)
		{
			codegen::type_info &t=v.type_;
			int n = codegen::cmp_type(a.type_.b_type, b.type_.b_type);
			if (n > 0) {
				t = a.type_;
				b = codegen::convert(b, t,false);
			}
			else if (n < 0)
			{
				t = b.type_;
				a = codegen::convert(a, t, false);
			}
			else {
				t = a.type_;
			}
			std::wstring str;
			if (op == L"*")
				str = L"mul";
			else if (op == L"/")
				str = L"div";
			std::wcout << v.real_name << "=" << str << " " << t << " " << a.real_name << "," << b.real_name << "\n";
		}
		else {
			throw "can not compute not number type";
		}
		return v;
	};
	const_exp = { const_exp,op1,const_exp }, [](tmp_var a, std::wstring op, tmp_var b) {
		tmp_var v = map.newvar();
		if (a.type_.type_type() == codegen::interger && b.type_.type_type() == codegen::interger)
		{
			codegen::type_info &t = v.type_;
			int n = codegen::cmp_type(a.type_.b_type, b.type_.b_type);
			if (n > 0) {
				t = a.type_;
				b = codegen::convert(b, t, false);
			}
			else if (n < 0)
			{
				t = b.type_;
				a = codegen::convert(a, t, false);
			}
			else {
				t = a.type_;
			}
			std::wstring str;
			if (op == L"+")
				str = L"add";
			else if (op == L"-")
				str = L"sub";
			std::wcout << v.real_name << "=" << str << " " << t << " " << a.real_name << "," << b.real_name << "\n";
		}
		else {
			throw "can not compute not number type";
		}
		return v;
	};
	const_exp = { exp,equal,const_exp }, [](addr_var a, not_use, tmp_var b) {
		a.save(convert(b, a.type_,false));
		return b;
	};
	root = { block },pass_by(0);
	root = { root,block }, pass_by(0);
	block = { statement }, pass_by(0);
	statement = { const_exp,sep }, pass_by(-1);
	//pass_by传-1表示不生成数据
	statements = { statement }, pass_by(-1);
	statements = { statements,statement }, pass_by(-1);
	auto a = make_dfa();
	bflag = 1;
	auto b = make_grammer(root, [](not_use a) {});
	//读取并执行test.txt的内容
	std::wifstream ff(arg_ar[1]);
	auto it = a->read(std::istreambuf_iterator<wchar_t>(ff), std::istreambuf_iterator<wchar_t>());
	std::cout << std::endl;
	b->read(it.first, it.second);
 	return 0;
}
