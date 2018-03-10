#include "dfa.h"
#include "yufashu.hpp"
#include <iostream>
#include <iterator>
#include <iterator>
using yacc::unit;
using yacc::unit_it;
using yacc::make_grammer;
using yacc::pass_by;
using yacc::not_use;
int main()
{
	final_symbol number(LR"((1-9)(0-9)*(.(0-9)*(1-9))?)");
	final_symbol op1(LR"(\+|\-)");
	final_symbol op2(LR"(\*|\/)");
	final_symbol bl(LR"(\()");
	final_symbol br(LR"(\))");
	final_symbol equal(LR"(\=)");
	final_symbol newline(LR"(\
)");
	symbol muti;
	symbol backet;
	symbol plus;
	symbol global;
	global = { plus,equal }, [](double a) {
		printf("%lf", a);
		return not_use();
	};;;
	global = { global,newline,global }, pass_by(-1);
	backet = { bl,backet,br }, pass_by(1);
	backet = { number }, [](std::wstring str) {
		return std::stod(str);
	};

	muti = { backet }, pass_by(0);
	muti={ muti,op2,muti }, [](double a,std::wstring op,double b) {
		switch (op[0])
		{
		case L'*':
			return a * b;
			break;
		case L'/':
			return a / b;
			break;
		}
	};
	plus ={muti}, pass_by(0);
	plus = { plus,op1,plus }, [](double a, std::wstring  op, double b) {
		switch (op[0])
		{
		case L'+':
			return a - b;
			break;
		case L'-':
			return a + b;
			break;
		}
	};
	auto a = make_dfa();
	auto b = make_grammer(global,[](not_use a) {});
	auto it = a->read(std::istreambuf_iterator<wchar_t>(std::wcin), std::istreambuf_iterator<wchar_t>());
	b->read(it.first, it.second);
}