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
#include "yufashu.h"
//输入为文法符号id
typedef size_t input_type;
//产生式右侧和语法制导翻译的函数
typedef std::pair<std::vector<input_type>, specification_handle> specification_left;

//lr1项目
class project
{
public:
	input_type from_id;
    const specification_left &to_id;
	input_type fin_id;
    size_t pos;
    project(input_type id_,const specification_left &to_id_,input_type fin_id_,size_t pos_=0);
	bool operator==(const project &a)const;
    project next() const;
    //void print();
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
typedef std::variant<shift, specification> op;
//状态转移表的索引
typedef std::pair<size_t, size_t> state_index;
//实现hash函数
namespace std
{
	template<> struct hash<state_index>
	{
		typedef state_index argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept;
	};
}
//项目为状态类型
typedef project state_type;
namespace std
{
	template<> struct hash<project>
	{
		typedef project argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept;
	};
}

class grammer_impl
{
	typedef std::unordered_map<state_index, op> state_map;
	state_map map;
	std::vector<unit> stack_symbol;
	std::vector<size_t> stack_state;
public:
	grammer_impl(state_map &&map_);
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
struct binder_impl
{
	std::vector<input_type> symbols;
	input_type id;
	binder_impl(input_type id_, std::vector<input_type> &&syms);
	void operator,(specification_handle han);
};
class grammer_maker:public state_to_map<state_type, input_type>
{
	typedef std::unordered_map<state_index, op> state_map;
public:
    typedef typename state_to_map<state_type,input_type>::state_set state_set;
    typedef typename state_to_map<state_type, input_type>::next_map next_map;
    std::unordered_multimap<input_type, specification_left> gram_map;
	state_map map;
    //std::stack<gram_tree_node> stack_id;
	grammer_maker()=default;
    void get_first(input_type id,std::set<input_type> &set);
    virtual void expand(state_type type,state_set &ptr);
    virtual void next(const state_set &ptr,next_map &map_);
    virtual void link(const state_set &from_set,const state_set &to_set,input_type input);
    virtual void add_state(size_t n,const state_set &ptr);
	void add(input_type a,std::vector<input_type> &&b, specification_handle handler);
	grammer_impl *make_grammer(input_type root_state, specification_handle root_handle);
    //void read(gram_tree_node b);
};

