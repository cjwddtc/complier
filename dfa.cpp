#include "dfa.h"
#include <map>
#include <algorithm>

class nfa
{
	//nfa的状态转移的输入或者说边optional表示可以包含wchar_t也可能为空，为空时表示为空边（可以直接转化）
	typedef std::optional<wchar_t> input_type;
	//某一个nfa状态描述所有的边,size_t为nfa状态在数组中的索引
	typedef std::unordered_multimap<input_type, size_t> edge_map;
	//这个optional表示此状态是否为可结束状态，如果是此optional储存结束时输出的词法单元id
	//这两个组合在一起作为nfa的状态
	typedef std::pair<edge_map, std::optional<size_t>> nfa_status;
	//当前正则的id,生成nfa是使用
	size_t c_id;
	//nfa状态数组
	std::unordered_map<wchar_t, symbol> id_map;
	//解析正则表达式的语法分析器
	std::shared_ptr<grammer> reggm;
	//创建一个nfa状态返回其在数组中的索引
	size_t create_status()
	{
		status.push_back(nfa_status());
		return status.size() - 1;
	}
	//设置一个nfa状态为可结束状态,结束时输出位name，该状态在数组中的索引为id
	void set_name(size_t id, size_t name)
	{
		status[id].second = name;
	}
	//添加一条nfa边,第三个参数默认为空边
	void link(size_t from, size_t to, input_type ch = input_type())
	{
		status[from].first.emplace(ch, to);
	}
protected:
	//nfa状态数组
	std::vector<nfa_status> status;
public:
	nfa();
	//讲一个正则表达式添加到nfa中
	void add(std::wstring str, size_t id);
};
namespace std
{
	//hash函数不重要
	hash<status_index>::result_type hash<status_index>::operator()(argument_type const& s) const noexcept
	{
		result_type const h1(std::hash<size_t>{}(s.first));
		result_type const h2(std::hash<size_t>{}(s.second));
		return h1 ^ h2;
	}
}
using yacc::unit_it;
using yacc::unit;
typedef std::pair<size_t, size_t> node_s;
using yacc::pass_by;
//这个函数主要调用语法分析器解析正则表达式,语法分析器使用方法会在main.cpp中描述
nfa::nfa()
{
	//这个语句主要是避开0状态作为初始状态
	create_status();
	symbol char_;//regularchar
	symbol minus;//-
	symbol left_small;//(
	symbol right_small;//)
	symbol star;//*
	symbol plus;//+
	symbol question;//?
	symbol backslash;//\ 
	symbol or ;//|
	symbol backslash_exp;//\a 
	symbol backet_exp;//(a)
	symbol repeat_exp;//*+?a
	symbol or_exp;//a|b
	symbol to_exp;//a-b
	id_map.emplace(L'(',left_small);
	id_map.emplace(L')', right_small);
	id_map.emplace(L'*', star);
	id_map.emplace(L'+', plus);
	id_map.emplace(L'-', minus);
	id_map.emplace(L'?', question);
	id_map.emplace(L'\\', backslash);
	id_map.emplace(L'|', or);
	using yacc::not_use;
	for (auto &b : id_map)
	{
		backslash_exp = { backslash,b.second }, [this](not_use,wchar_t ch) {
			size_t start_s = create_status();
			size_t end_s = create_status();
			link(start_s, end_s,ch);
			return std::make_pair(start_s, end_s);
		};
	}
	backslash_exp = { backslash,char_}, [this](not_use ,wchar_t ch) {
		size_t start_s = create_status();
		size_t end_s = create_status();
		link(start_s, end_s, ch);
		return std::make_pair(start_s, end_s);
	};
	to_exp = { char_,minus,char_ }, [this](wchar_t a,not_use,wchar_t b)
	{
		size_t start_s = create_status();
		size_t end_s = create_status();
		while (a <= b)
		{
			link(start_s, end_s, a++);
		}
		return std::make_pair(start_s, end_s);
	};
	backet_exp = { left_small ,or_exp,right_small }, pass_by(1);
	backet_exp = {backslash_exp}, pass_by(0);
	backet_exp = { char_ }, [this](wchar_t ch) {
		size_t start_s = create_status();
		size_t end_s = create_status();
		link(start_s, end_s,ch);
		return std::make_pair(start_s, end_s);
	};
	backet_exp = { to_exp }, pass_by(0);
	repeat_exp = { backet_exp,star }, [this](node_s a,not_use) {
		link(a.second, a.first);
		link(a.first, a.second);
		return a;
	};
	repeat_exp = { backet_exp,plus }, [this](node_s a, not_use) {
		link(a.second, a.first);
		return a;
	};
	repeat_exp = { backet_exp,question }, [this](node_s a, not_use) {
		link(a.first, a.second);
		return a;
	};
	repeat_exp = {backet_exp}, pass_by(0);
	or_exp = { or_exp,or,or_exp }, [this](node_s a,not_use,node_s b) {
		link(a.first, b.first);
		link(b.second, a.second);
		return a;
	};
	or_exp = { repeat_exp }, pass_by(0);
	or_exp = { or_exp,or_exp }, [this](node_s a,node_s b) {
		link(a.second, b.first);
		return std::make_pair(a.first, b.second);
	};
	
	reggm = make_grammer(or_exp,[this](node_s b) {
		link(0, b.first);
		set_name(b.second, c_id);
	});
}

void nfa::add(std::wstring str, size_t id)
{
	std::vector<unit> m;
	m.reserve(str.size());
	std::transform(str.begin(), str.end(),std::back_inserter(m), [this](wchar_t ch) {
		auto it = id_map.find(ch);
		if (it != id_map.end())
		{
			return std::make_pair(it->second.id, std::any(ch));
		}
		else {
			return std::make_pair(1uLL, std::any(ch));
		}
	});
	c_id=id;
	reggm->read(m.begin(), m.end());
}
//nfa转dfa类，非常复杂，一方面继承自需要转换的nfa
//另外一方面集成了一个特殊的类,该类通过调用一组由子类重写的函数来实现构造状态转移表的功能
//具体请看template.hpp
//只需要实现预定义好的闭包（expand）和状态转移（next）函数（即求这个状态所能转移到的所有状态）既可以实现nfa转dfa
class dfa_maker :public state_to_map<state_type, input_type>, public nfa
{
public:
	typedef typename state_to_map<state_type, wchar_t>::state_set state_set;
	typedef typename state_to_map<state_type, wchar_t>::next_map next_map;
	std::unordered_map<status_index, size_t> status_map;
	std::unordered_map<state_type, size_t> fin_status;
	virtual void expand(state_type id, state_set &ptr)
	{
		auto &stat = status[id].first;
		auto b = stat.equal_range(std::optional<wchar_t>());
		while (b.first != b.second)
		{
			if (ptr.find(b.first->second) == ptr.end())
			{
				ptr.emplace(b.first->second);
				expand(b.first->second, ptr);
			}
			++b.first;
		}
	}
	virtual void next(const state_set &ptr, next_map &map)
	{
		for (size_t b : ptr)
		{
			for (auto a : status[b].first)
			{
				if (a.first) {
					map[*a.first].insert(a.second);
				}
			}
		}
	}
	//state_to_map在创建一条新边的时候调用
	virtual void link(const state_set &from_set, const state_set &to_set, wchar_t ch)
	{
		status_map[std::make_pair(this->get_id(from_set), ch)] = this->get_id(to_set);
	}
	//state_to_map在新建状态是调用
	virtual void add_state(size_t n, const state_set &ptr)
	{
		//判断这个状态是否是可结束状态表
		for (size_t d : ptr)
		{
			if (status[d].second)
			{
				fin_status[n] = *status[d].second;
			}
		}
	}
	//调用父类make_map nfa转dfa
	std::shared_ptr<dfa> make_dfa()
	{
		make_map(0);
		return std::make_shared<dfa>(std::move(status_map), std::move(fin_status));
	}
};
//全局的一个dfa_maker
thread_local dfa_maker global_dfa;

std::shared_ptr<dfa> make_dfa()
{
	return global_dfa.make_dfa();
}

final_symbol::final_symbol(std::wstring str)
{
	global_dfa.add(str, this->id);
}

dfa::dfa(std::unordered_map<status_index, size_t>&& m, std::unordered_map<state_type, size_t>&& f)
	:status_map(std::move(m)),fin_status(std::move(f))
{
}

null_symbol::null_symbol(std::wstring str)
{
	global_dfa.add(str, 0);
}
