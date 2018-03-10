#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <assert.h>
#include <set>
#include <stack>
#include "template.hpp"
#include <string>
#include <variant>
#include <functional>
#include <any>

//lr1项目
namespace yacc {
	using ::std::pair;
	using ::std::vector;
	using ::std::function;
	using ::std::any;
	using ::std::variant;
	//输入为文法符号id
	typedef size_t input_type;
	//产生式右侧和语法制导翻译的函数
	typedef pair<const size_t, any> unit;
	typedef vector<any>::iterator unit_it;
	typedef function<any(unit_it start)> specification_handle;
	typedef pair<vector<input_type>, specification_handle> specification_left;
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
	//状态转移表的索引
	typedef pair<size_t, size_t> state_index;
	struct not_use {};
}

//实现hash函数
namespace std
{
	template<> struct hash<yacc::state_index>
	{
		typedef yacc::state_index argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			result_type const h1(std::hash<size_t>{}(s.first));
			result_type const h2(std::hash<size_t>{}(s.second));
			return h1 ^ h2;
		}
	};
}
namespace tmp
{
	using std::any;
	template <class T,class ...ARG>
	any invoke(std::function < T(yacc::not_use,ARG...) > func, yacc::unit_it a)
	{
		std::function<T(ARG...)> f = [func, a](ARG ... arg) {return func(yacc::not_use(), arg...);};
		return invoke(f, a + 1);
	}
	template <class T, class F, class ...ARG>
	any invoke(std::function < T(F, ARG...) > func, yacc::unit_it a)
	{
		std::function<T(ARG...)> f = [func, a](ARG ... arg) {return func(std::any_cast<F>(a[0]), arg...); };
		return invoke(f, a + 1);
	}
	template <class T>
	any invoke(std::function < T() > func, yacc::unit_it a)
	{
		return func();
	}
	using namespace std;
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
	class gammer
	{
		typedef std::unordered_map<state_index, op> state_map;
		state_map map;
		std::vector<any> stack_symbol;
		std::vector<size_t> stack_state;
	public:
		gammer(state_map &&map_);
		void read_one(unit a);
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
			read_one(std::make_pair(-1, std::any()));
		}
	};
	struct pass_by
	{
		size_t n;
		pass_by(size_t m);
	};
	class binder
	{
		std::vector<input_type> symbols;
		input_type id;
		void add(specification_handle han);
	public:
		binder(input_type id_, std::vector<input_type> &&syms);
		void operator,(pass_by p);
		template <class T>
		void operator,(T han)
		{
			add([han](unit_it a) {return tmp::invoke(tmp::FFL(han), a); });
		}
	};
	struct symbol
	{
		size_t id;
		symbol();
		binder operator=(std::initializer_list<symbol> symbols);
		bool operator==(const symbol &)const;
	};
	std::shared_ptr<gammer> make_grammer(symbol sym, specification_handle root_handle);
	template <class T>
	std::shared_ptr<gammer> make_grammer(symbol sym, std::function<void(T)> root_handle)
	{
		specification_handle b = [root_handle](unit_it i) {
			root_handle(std::any_cast<T>(*i));
			return any();
		};
		return make_grammer(sym, b);
	}
	template <class T>
	std::shared_ptr<gammer> make_grammer(symbol sym, T root_handle)
	{
		return make_grammer(sym,tmp::FFL(root_handle));
	}
}
