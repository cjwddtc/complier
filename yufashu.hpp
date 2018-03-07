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
	typedef vector<unit>::iterator unit_it;
	typedef function<void(unit_it start, unit_it end, unit&)> specification_handle;
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
namespace yacc{
	class gammer
	{
		typedef std::unordered_map<state_index, op> state_map;
		state_map map;
		std::vector<unit> stack_symbol;
		std::vector<size_t> stack_state;
	public:
		gammer(state_map &&map_);
		void read_one(unit a);
		template <class ITERATOR>
		void read(ITERATOR start, ITERATOR end)
		{
			while (start != end)
			{
				read_one(*start++);
			}
			read_one(std::make_pair(-1, std::any()));
		}
	};
	struct binder
	{
		std::vector<input_type> symbols;
		input_type id;
		binder(input_type id_, std::vector<input_type> &&syms);
		void operator,(specification_handle han);
	};
	struct symbol
	{
		size_t id;
		symbol();
		binder operator=(std::initializer_list<symbol> symbols);
		bool operator==(const symbol &)const;
	};
	std::shared_ptr<gammer> make_grammer(symbol sym, specification_handle root_handle);

}
