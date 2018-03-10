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
using yacc::symbol;
using yacc::unit;


typedef size_t state_type;
typedef std::pair<state_type, wchar_t> status_index;
typedef  wchar_t input_type;
//ÊµÏÖhashº¯Êý
namespace std
{
	template<> struct hash<status_index>
	{
		typedef status_index argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept;
	};
}

struct dfa
{
	std::unordered_map<status_index, size_t> status_map;
	std::unordered_map<state_type, size_t> fin_status;
	dfa(std::unordered_map<status_index, size_t> &&m, std::unordered_map<state_type, size_t> &&f);
	template <class IT>
	struct word_iterator
	{
		IT cu;
		IT end;
		dfa *d;
		size_t current_status;
		std::wstring value;
		size_t id;
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
					auto it = d->fin_status.find(current_status);
					if ( it != d->fin_status.end()) {
						id = it->second;
						current_status = 0;
						return *this;
					}
					else {
						printf("%d\n", *cu);
						throw *cu;
					}
				}
				++cu;
			}
			if (current_status == 0)
			{
				if (flag)
				{
					current_status = -1;
				}
				return *this;
			}
			throw 1;
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


std::shared_ptr<dfa> make_dfa();

struct final_symbol :public symbol
{
	final_symbol(std::wstring str);
};

