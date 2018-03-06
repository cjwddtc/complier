#include "dfa.h"
#include "yufashu.h"
namespace std
{
hash<status_index>::result_type hash<status_index>::operator()(argument_type const& s) const noexcept
{
	result_type const h1(std::hash<size_t>{}(s.first));
	result_type const h2(std::hash<size_t>{}(s.second));
	return h1 ^ h2;
}
}
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
	symbol nor;//^
	symbol or ;//|
	symbol backslash_exp;//\a 
	symbol backet_exp;//(a)
	symbol repeat_exp;//*+?a
	symbol or_exp;//a|b
	symbol to_exp;//a-b
	std::unordered_map<symbol, wchar_t> id_map;
	id_map[left_small] = L'(';
	id_map[right_small] = L')';
	id_map[star] = L'*';
	id_map[plus] = L'+';
	id_map[minus] = L'-';
	id_map[backslash] = L'\\';
	id_map[or ] = L'|';
	for (auto &b : id_map)
	{
		backslash_exp = { backslash,b.first }, [ch = b.second, this](unit_it start, unit_it end, unit&one) {
			size_t start_s = create_status();
			size_t end_s = create_status();
			link(start_s, end_s, ch);
			one.second = std::make_pair(start_s, end_s);
		};
	}
	backslash_exp = { char_ }, [this](unit_it start, unit_it end, unit&one) {
		size_t start_s = create_status();
		size_t end_s = create_status();
		link(start_s, end_s, std::any_cast<wchar_t>(start->second));
		one.second = std::make_pair(start_s, end_s);
	};
	backet_exp = { left_small ,backslash_exp,right_small }, [](unit_it start, unit_it end, unit&one) {
		one.second = start[1].second;
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
	backet_exp = {backslash_exp}, [](unit_it start, unit_it end, unit&one) {
		one.second = start[0].second;
	};
	backet_exp = { to_exp }, [](unit_it start, unit_it end, unit&one) {
		one.second = start[0].second;
	};
	repeat_exp = { star,backet_exp }, [this](unit_it start, unit_it end, unit&one) {
		auto a = std::any_cast<node_s>(start[0].second);
		link(a.second, a.first);
		link(a.first, a.second);
		one.second = std::make_pair(a.first, a.second);
	};
	repeat_exp = { plus,backet_exp }, [this](unit_it start, unit_it end, unit&one) {
		auto a = std::any_cast<node_s>(start[0].second);
		link(a.second, a.first);
		one.second = std::make_pair(a.first, a.second);
	};
	repeat_exp = { question,backet_exp }, [this](unit_it start, unit_it end, unit&one) {
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
}

void nfa::add(std::wstring str, size_t id)
{

}