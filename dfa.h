/*
这个文件实现了通过lr(1)对正则表达式读取并转换成nfa在转换成dfa
*/
#pragma once
#include <unordered_map>
#include "template.hpp"
#include <vector>
#include <assert.h>
#include <memory>
#include <optional>
//#define DEBUG_MAP

#include "yufashu.hpp"
using grammer = yacc::gammer;
using yacc::symbol_impl;
using yacc::unit;
using output_type = yacc::input_type;

typedef size_t state_type;
typedef std::pair<state_type, wchar_t> status_index;
typedef  wchar_t input_type;
//实现hash函数，不重要
namespace std
{
	template<> struct hash<status_index>
	{
		typedef status_index argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept;
	};
}

constexpr std::string_view ignore_id = "__ignore__";
struct dfa
{
	//状态转移表
	std::unordered_map<status_index, size_t> status_map;
	//可结束状态表(在这些状态是接受不可接受输入视为解析到一个词法单元
	std::unordered_map<state_type, output_type> fin_status;
	dfa(std::unordered_map<status_index, size_t> &&m, std::unordered_map<state_type, output_type> &&f);
	//使用了一些特殊的奇技淫巧实现了将一对字符流迭代器转化成词法符号迭代器并直接传给语法分析器，不重要
	template <class IT>
	struct word_iterator
	{
		IT cu;
		IT end;
		dfa *d;
		size_t current_status;
		std::wstring value;
		output_type id;
		word_iterator(IT start_, IT end_, dfa *f):
			cu(start_), end(end_), d(f),current_status(0){
			if (cu == end) current_status = -1;
		}
		unit operator*()
		{
			return unit(id, value);
		}
		bool operator!=(const word_iterator&o)
		{
			return current_status != o.current_status;
		}
		word_iterator &try_finish() {
			auto it = d->fin_status.find(current_status);
			if (it != d->fin_status.end()) {
				id = it->second;
				current_status = 0;
				if (id == ignore_id) {
					operator++();
				}
				return *this;
			}
			else {
				printf("%d\n", *cu);
				throw *cu;
			}
		}
		word_iterator &operator++()
		{
			value.clear();
			bool flag = true;
			while (cu != end) {
				flag = false;
				auto it = d->status_map.find(status_index(current_status, *cu));
				if (it != d->status_map.end())
				{
					current_status = it->second;
					value.push_back(*cu);
				}
				else {
					return try_finish();
				}
				++cu;
			}
			if (flag) {
				current_status = -1;
				return *this;
			}
			else {
				return try_finish();
			}
		}
		word_iterator operator++(int)
		{
			word_iterator<IT> it(*this);
			this->operator++();
			return it;
		}
	};
	template <class IT>
	std::pair<word_iterator<IT>, word_iterator<IT>> read(IT start, IT end)
	{
		word_iterator<IT> it(start, end, this);
		++it;
		return std::make_pair(it, word_iterator<IT>(end, end, this));
	}
};

//通过nfa创建dfa
std::shared_ptr<dfa> make_dfa();
//这个类由final_symbol和null_symbol继承，在保证在构造symbol对线之前全局nfa已被构造
struct force_init {
	force_init();
};
//终结符，通过词法分析器输出的符号继承自非终结符
struct final_symbol_impl :force_init,public symbol_impl
{
	final_symbol_impl(std::wstring str,std::string id);
};
//空字符，词法分析器不会输出这些词法单元（比如空格回车等等）
struct null_symbol_impl :force_init,public symbol_impl
{
	null_symbol_impl(std::wstring str);
};
#define final_symbol(a,b) final_symbol_impl a(b,#a)
#define null_symbol(a,b) null_symbol_impl a(b)