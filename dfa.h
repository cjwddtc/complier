#pragma once
#include <map>
#include "predef.h"
#include "map_two.hpp"
#include "template.hpp"
#include "gram_tree.h"
#include "regex.hpp"
#include <set>
#include <vector>
#include <assert.h>
#include <memory>
#include <iostream>
#include <iterator>
#include <optional>
//#define DEBUG_MAP


class nfa
{
	typedef std::optional<wchar_t> input_type;
	typedef std::unordered_multimap<input_type, size_t> edge_map;
	typedef std::pair<edge_map, std::optional<size_t>> nfa_status;


	size_t create_status()
	{
		status.push_back(nfa_status());
		return status.size() - 1;
	}
	void set_name(size_t id, size_t name)
	{
		status[id].second = name;
	}
	void link(size_t from, size_t to, input_type ch=input_type())
	{
		status[from].first.emplace(ch, to);
	}
protected:
	std::vector<nfa_status> status;
public:
	nfa();
	void add(std::wstring str,size_t id);
};


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
class dfa:public state_to_map<state_type, input_type>,nfa
{
public:
	typedef typename state_to_map<state_type, wchar_t>::state_set state_set;
	typedef typename state_to_map<state_type, wchar_t>::next_map next_map;
	std::unordered_map<status_index, size_t> status_map;
	std::unordered_map<state_type, size_t> fin_status;
    virtual void expand(state_type id,state_set &ptr)
    {
		auto &stat = status[id].first;
		auto b = stat.equal_range(std::optional<wchar_t>());
        while(b.first!=b.second)
        {
            if(ptr.find(b.first->second)==ptr.end())
            {
				ptr.emplace(b.first->second);
                expand(b.first->second,ptr);
            }
            ++b.first;
        }
    }
    virtual void next(const state_set &ptr,next_map &map)
    {
        for(size_t b:ptr)
        {
            for(auto a: status[b].first)
            {
                map[*a.first].insert(a.second);
            }
        }
    }

    virtual void link(const state_set &from_set,const state_set &to_set,wchar_t ch)
    {
		status_map[std::make_pair(this->get_id(from_set),ch)]= this->get_id(to_set);
    }
    virtual void add_state(size_t n,const state_set &ptr)
    {
        for(size_t d:ptr)
        {
			if (status[d].second)
			{
				fin_status[n] = *status[d].second;
			}
        }
    }
};
thread_local dfa f;

