//语法分析器
#pragma once
//#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <assert.h>
#include <stack>
#include "template.hpp"
#include <string>
#include <variant>
#include <functional>
#include <any>

//lr1项目
namespace yacc {
	constexpr std::string_view fin_id = "__end__";
	constexpr std::string_view root_id = "__root__";
	using ::std::pair;
	using ::std::vector;
	using ::std::function;
	using ::std::any;
	using ::std::variant;
	//输入为文法符号id
	typedef std::string input_type;
	//产生式右侧和语法制导翻译的函数
	//每个文法符号都有一个值用于语法制导翻译由于类型不同存为any
	typedef pair<input_type, any> unit;
	typedef vector<any>::iterator unit_it;
	//规约时的触发函数
	typedef function<any(unit_it start)> specification_handle;
	//一个产生式的左端
	struct symbol_impl;
	struct pass_by;
	struct right_combin {};
	struct specification_left
	{
		//产生是右端的符号
		vector<input_type> values;
		//规约处理函数
		specification_handle handler;
		//优先级，其中除了最低位bit以外表示优先级,越小优先级越高，最低位表示是否为有结合或者说在同一优先级下发生移进规约冲突时是否移进
		uint64_t level;
		specification_left &operator,(pass_by p);
		template <class T>
		specification_left &operator,(T han);
		specification_left &operator,(right_combin);
	};
	//移进
	struct shift
	{
		size_t state;
	};
	//规约
	struct specification
	{
		size_t size;
		input_type id;
		specification_handle func;
	};
	//移近或是规约
	typedef variant<shift, specification> op;
	//状态转移表的索引（当前状态，输入文法符号
	typedef pair<size_t, input_type> state_index;
	//工具类不重要
	struct not_use {};
}

//实现hash函数，不重要
namespace std
{
	template<> struct hash<yacc::state_index>
	{
		typedef yacc::state_index argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			result_type const h1(std::hash<size_t>{}(s.first));
			result_type const h2(std::hash<yacc::input_type>{}(s.second));
			return h1 ^ h2;
		}
	};
}
//将变量名作为符号的名字
#define symbol(a) symbol_impl a(#a)
//工具类通过一些奇技淫巧简化代码编写，不重要
namespace tmp
{
	using std::any;
	any invoke(std::function < void() > func, yacc::unit_it a);
	template <class T>
	any invoke(std::function < T() > func, yacc::unit_it a)
	{
		return func();
	}
	template <class T,class ...ARG>
	any invoke(std::function < T(yacc::not_use,ARG...) > func, yacc::unit_it a);
	template <class T, class F, class ...ARG>
	any invoke(std::function < T(F, ARG...) > func, yacc::unit_it a)
	{
		std::function<T(ARG...)> f = [func, a](ARG ... arg) {return func(std::any_cast<F>(a[0]), arg...); };
		return invoke(f, a + 1);
	}
	template <class T,class ...ARG>
	any invoke(std::function < T(yacc::not_use,ARG...) > func, yacc::unit_it a)
	{
		std::function<T(ARG...)> f = [func, a](ARG ... arg) {return func(yacc::not_use(), arg...);};
		return invoke(f, a + 1);
	}
	template<typename T>
	struct memfun_type
	{
		using type = void;
	};
	template<typename Ret, typename Class, typename... Args>
	struct memfun_type<Ret(Class::*)(Args...) const>
	{
		using type = std::function<Ret(Args...)>;
	};
	template<typename F>
	typename memfun_type<decltype(&F::operator())>::type
		FFL(F const &func)
	{//Function from lambda!
		return func;
	}
}
namespace yacc{
	template <class T>
	specification_left &specification_left::operator,(T han)
	{
		handler = [han](unit_it a) {
			return tmp::invoke(tmp::FFL(han), a); };
		return *this;
	}
	//词法分析器
	class gammer
	{
		typedef std::unordered_map<state_index, op> state_map;
		//状态转移表
	public:
		state_map map;
		//数据栈储存每个文法符号的数据any可以储存任意类型
		std::vector<any> stack_symbol;
		//状态栈
		std::vector<size_t> stack_state;
	public:
		gammer(state_map &&map_);
		//读取一个符号
		void read_one(unit a);
		//读取从start到end的单元并在末尾加一个结束符
		template <class ITERATOR>
		void read(ITERATOR start, ITERATOR end)
		{
			stack_state.clear();
			stack_symbol.clear();
			stack_state.emplace_back(0);
			while (start != end)
			{
				read_one(*start++);
			}
			read_one(std::make_pair(std::string(yacc::fin_id), std::any()));
		}
	};
	//辅助类表示这次规约只是单纯的将第n个被规约符号的数据作为规约成的新文法符号的数据
	struct pass_by
	{
		size_t n;
		pass_by(size_t m);
	};
	//文法符号
	struct symbol_impl
	{
		std::string id;
		symbol_impl(std::string name);
		//通过赋值运算符表示一个产生式具体用法见cpp文件末尾的main_文件
		specification_left &operator=(const std::initializer_list<symbol_impl> &lil);
		bool operator==(const symbol_impl &)const;
	};
	//下面全部都是一些特殊的奇技淫巧，不重要，主要是根据用户传入的函数自动的将对应的类型从any中取出
	std::shared_ptr<gammer> make_grammer(symbol_impl sym, specification_handle root_handle);
	template <class T>
	std::shared_ptr<gammer> make_grammer(symbol_impl sym, std::function<void(T)> root_handle)
	{
		specification_handle b = [root_handle](unit_it i) {
			root_handle(std::any_cast<T>(*i));
			return any();
		};
		return make_grammer(sym, b);
	}
	std::shared_ptr<gammer> make_grammer(symbol_impl sym, std::function<void(not_use)> root_handle);
	template <class T>
	std::shared_ptr<gammer> make_grammer(symbol_impl sym, T root_handle)
	{
		return make_grammer(sym,tmp::FFL(root_handle));
	}
}
extern int bflag;