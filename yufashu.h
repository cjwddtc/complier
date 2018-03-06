#pragma once
#include <memory>
#include <functional>
#include <unordered_map>
#include <any>
struct grammer_impl;;
struct binder_impl;
//文法符号和对应数据
typedef std::pair<const size_t, std::any> unit;
typedef std::vector<unit>::iterator unit_it;
//语法制导翻译的函数
typedef std::function<void(unit_it start, unit_it end, unit&)> specification_handle;
struct grammer
{
	std::shared_ptr<grammer_impl>_imp;
	grammer(grammer_impl *p);
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
	std::shared_ptr<binder_impl>_imp;
	binder(binder_impl *p);
	void operator,(specification_handle handle);
};
struct symbol
{
	size_t id;
	symbol();
	binder operator=(std::initializer_list<symbol> symbols);
	bool operator==(const symbol &)const ;
};
grammer make_grammer(symbol sym);

namespace std
{
	template<> struct hash<symbol>
	{
		typedef symbol argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept;
	};
}