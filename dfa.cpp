#include "dfa.h"
#include <algorithm>
namespace std
{
	hash<status_index>::result_type hash<status_index>::operator()(argument_type const& s) const noexcept
	{
		result_type const h1(std::hash<size_t>{}(s.first));
		result_type const h2(std::hash<size_t>{}(s.second));
		return h1 ^ h2;
	}
}
template <class IT>
struct regex_reader:public IT
{
	const std::map<symbol, wchar_t> &mp;
	symbol operator*()
	{
		if(**this).
		return (**this)
	}
};
using yacc::unit_it;
using yacc::unit;
typedef std::pair<size_t, size_t> node_s;
nfa::nfa()
{
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
	for (auto &b : id_map)
	{
		backslash_exp = { backslash,b.second }, [this](unit_it start, unit_it end, unit&one) {
			size_t start_s = create_status();
			size_t end_s = create_status();
			link(start_s, end_s, std::any_cast<wchar_t>(start[1].second));
			one.second = std::make_pair(start_s, end_s);
		};
	}
	backslash_exp = { backslash,char_}, [this](unit_it start, unit_it end, unit&one) {
		size_t start_s = create_status();
		size_t end_s = create_status();
		link(start_s, end_s, std::any_cast<wchar_t>(start[1].second));
		one.second = std::make_pair(start_s, end_s);
	};
	to_exp = { char_,minus,char_ }, [this](unit_it start, unit_it end, unit&one)
	{
		size_t start_s = create_status();
		size_t end_s = create_status();
		wchar_t a = std::any_cast<wchar_t>(start[0].second);
		wchar_t b = std::any_cast<wchar_t>(start[2].second);
		while (a <= b)
		{
			link(start_s, end_s, a++);
		}
		one.second = std::make_pair(start_s, end_s);
	};
	backet_exp = { left_small ,or_exp,right_small }, [](unit_it start, unit_it end, unit&one) {
		one.second = start[1].second;
	};
	backet_exp = {backslash_exp}, [](unit_it start, unit_it end, unit&one) {
		one.second = start[0].second;
	};
	backet_exp = { char_ }, [this](unit_it start, unit_it end, unit&one) {
		size_t start_s = create_status();
		size_t end_s = create_status();
		link(start_s, end_s, std::any_cast<wchar_t>(start->second));
		one.second = std::make_pair(start_s, end_s);
	};
	backet_exp = { to_exp }, [](unit_it start, unit_it end, unit&one) {
		one.second = start[0].second;
	};
	repeat_exp = { backet_exp,star }, [this](unit_it start, unit_it end, unit&one) {
		auto a = std::any_cast<node_s>(start[0].second);
		link(a.second, a.first);
		link(a.first, a.second);
		one.second = std::make_pair(a.first, a.second);
	};
	repeat_exp = { backet_exp,plus }, [this](unit_it start, unit_it end, unit&one) {
		auto a = std::any_cast<node_s>(start[0].second);
		link(a.second, a.first);
		one.second = std::make_pair(a.first, a.second);
	};
	repeat_exp = { backet_exp,question }, [this](unit_it start, unit_it end, unit&one) {
		auto a = std::any_cast<node_s>(start[0].second);
		link(a.first, a.second);
		one.second = std::make_pair(a.first, a.second);
	};
	repeat_exp = {backet_exp}, [](unit_it start, unit_it end, unit&one) {
		one.second = start[0].second;
	};
	or_exp = { repeat_exp,or,repeat_exp }, [this](unit_it start, unit_it end, unit&one) {
		auto a = std::any_cast<node_s>(start[0].second);
		auto b = std::any_cast<node_s>(start[2].second);
		link(a.first, b.first);
		link(b.second, b.first);
		one.second = a;
	};
	or_exp = { repeat_exp }, [](unit_it start, unit_it end, unit&one) {
		one.second = start[0].second;
	};
	or_exp = { or_exp,or_exp }, [this](unit_it start, unit_it end, unit&one) {
		auto a = std::any_cast<node_s>(start[0].second);
		auto b = std::any_cast<node_s>(start[1].second);
		link(a.second, b.first);
		one.second = std::make_pair(a.first, b.second);
	};
	reggm = make_grammer(or_exp, [this](unit_it start, unit_it end, unit&one) {
		node_s b = std::any_cast<node_s>(start->second);
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
	id = c_id;
	reggm->read(m.begin(), m.end());
}
int main()
{
	dfa_maker b;
	b.add(LR"(a+b|a)", 0);
	b.add(LR"(a*d|\ )", 1);
	b.add(LR"(b?d|w)", 2);
	
}

//thread_local dfa_maker global_dfa;
thread_local size_t re_s_id=0;

final_symbol::final_symbol(std::wstring str)
{
	//global_dfa.add(str, re_s_id++);
}
