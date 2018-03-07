#include <iostream>
#include <algorithm>
#include "yufashu.hpp"
using namespace yacc;

class project
{
public:
	input_type from_id;
	const specification_left &to_id;
	input_type fin_id;
	size_t pos;
	project(input_type id_, const specification_left &to_id_, input_type fin_id_, size_t pos_ = 0) :
		from_id(id_), to_id(to_id_), fin_id(fin_id_), pos(pos_) {}
	bool operator==(const project &a)const
	{
		return &to_id == &a.to_id&&fin_id == a.fin_id&&pos == a.pos;
	}
	project next() const
	{
		return project(from_id, to_id, fin_id, pos + 1);
	}
	//void print();
};
namespace std
{
	template<> struct hash<project>
	{
		typedef project argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			result_type const h2(std::hash<size_t>{}(s.fin_id));
			result_type const h3(std::hash<const char*>{}((const char *)&s.to_id));
			result_type const h4(std::hash<size_t>{}(s.pos));
			return h2 ^ h3^h4;
		}
	};
}

//项目为状态类型
typedef project state_type;

gammer::gammer(state_map &&map_) :map(std::move(map_)){
	stack_state.push_back(0);
}

void gammer::read_one(unit a)
{
	printf("%lld in:", a.first);
	for (auto a : stack_symbol)
	{
		printf("%lld|", a.first);
	}
	printf("\n");
	if (stack_symbol.size() == 1 && a.first == -1)
	{
		printf("");
	}
	auto it = map.find(state_index(stack_state.back(), a.first));
	if (it != map.end())
	{
		std::visit([this, a](auto arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, shift>)
			{
				stack_symbol.emplace_back(a);
				stack_state.emplace_back(arg.state);
			}
			else if constexpr (std::is_same_v<T, specification>)
			{
				printf("guiyue:%lld->%lld\n", arg.size, arg.id);
				unit b(arg.id, std::any());
				arg.func(stack_symbol.end() - arg.size, stack_symbol.end(), b);
				stack_state.resize(stack_state.size() - arg.size);
				stack_symbol.resize(stack_symbol.size() - arg.size);
				//stack_state.erase(stack_state.end() - arg.size, stack_state.end());
				if (arg.id == 0 && a.first == -1)
				{
					stack_state.clear();
					stack_state.emplace_back(0);
					stack_symbol.clear();
				}
				else {
					read_one(b);
					read_one(a);
				}
			}
			else {
				assert("this varint must be specification or shift");
			}
		}, it->second);
	}
	else {
		throw a;
	}
;
}

class grammer_maker :public state_to_map<state_type, input_type>
{
	typedef std::unordered_map<state_index, op> state_map;
public:
	typedef typename state_to_map<state_type, input_type>::state_set state_set;
	typedef typename state_to_map<state_type, input_type>::next_map next_map;
	std::unordered_multimap<input_type, specification_left> gram_map;
	state_map map;
	//std::stack<gram_tree_node> stack_id;
	grammer_maker() = default;
	void get_first(input_type id, std::set<input_type> &set)
	{
		if (gram_map.find(id) == gram_map.end())
		{
			set.insert(id);
		}
		else
		{
			auto itp = gram_map.equal_range(id);
			bool flag = true;
			for (auto it = itp.first; it != itp.second; it++)
			{
				if (it->second.first.front() == id) {
					break;
				}
				else {
					get_first(it->second.first.front(), set);
				}
			}
		}
	}
	virtual void expand(state_type type, state_set &ptr)
	{
		if (type.pos != type.to_id.first.size())
		{
			input_type other = type.to_id.first[type.pos];
			auto itp = gram_map.equal_range(other);
			std::set<input_type> set;
			if (type.pos + 1 == type.to_id.first.size())
			{
				set.insert(type.fin_id);
			}
			else {
				get_first(type.to_id.first[type.pos + 1], set);
			}
			for (auto it = itp.first; it != itp.second; it++)
			{
				for (auto id : set)
				{
					project prd(it->first, it->second, id);
					if (ptr.find(prd) == ptr.end())
					{
						ptr.insert(prd);
						expand(prd, ptr);
					}
				}
			}
		}
	}
	virtual void next(const state_set &ptr, next_map &map_)
	{
		for (const state_type &a : ptr)
		{
			if (a.to_id.first.size() == a.pos)
			{
				specification s;
				s.id = a.from_id;
				s.size = a.to_id.first.size();
				s.func = a.to_id.second;
				map[std::make_pair(this->get_id(ptr), a.fin_id)] = s;
			}
			else
			{
				auto b = a.to_id.first[a.pos];
				map_[b].insert(a.next());
			}
		}
	}
	virtual void link(const state_set &from_set, const state_set &to_set, input_type input)
	{
		shift s;
		s.state = this->get_id(to_set);
		map[std::make_pair(this->get_id(from_set), input)] = s;
	}

	virtual void add_state(size_t n, const state_set &ptr){}
	void add(input_type a, std::vector<input_type> &&b, specification_handle handler)
	{
		gram_map.emplace(a, std::make_pair(std::move(b), handler));
	}
	std::shared_ptr<gammer> make_grammer(input_type root_state, specification_handle root_handle)
	{
		auto b = gram_map.emplace(0, std::make_pair(std::vector<size_t>({ root_state }), root_handle));
		this->make_map(project(0, b->second, -1));
		gram_map.clear();
		return std::make_shared<gammer>(std::move(map));
	}
	//void read(gram_tree_node b);
};

thread_local grammer_maker global_grammer_impl;
thread_local size_t sid = 1;

std::shared_ptr<gammer> yacc::make_grammer(symbol sym, specification_handle root_handle)
{
	sid = 1;
	return global_grammer_impl.make_grammer(sym.id, root_handle);
}


symbol::symbol():id(sid++)
{
}

binder symbol::operator=(std::initializer_list<symbol> symbols)
{
	std::vector<input_type> vec(symbols.size());
	std::transform(symbols.begin(), symbols.end(), vec.begin(), [](auto a) {return a.id; });
	return binder(id,std::move(vec));
}

bool symbol::operator==(const symbol &o) const
{
	return id==o.id;
}

binder::binder(input_type id_, std::vector<input_type>&& syms):id(id_), symbols(std::move(syms))
{
	
}

void binder::operator,(specification_handle han)
{
	global_grammer_impl.add(id, std::move(symbols), han);
}


int main_() {/*
	symbol a;
	symbol b;
	symbol c;
	symbol d;
	symbol e;
	symbol f;
	specification_handle func = [](auto start, auto b, auto &c) {

	};
	a = { b,c,d }, func;
	a = { c,d }, func;
	a = { b,c,d,f }, func;
	b = { f }, func;
	c = { e,f }, func;
	std::shared_ptr<gammer> asd = make_grammer(a, specification_handle());
	std::vector<unit> vec;
	vec.push_back(std::make_pair(6, std::any()));
	vec.push_back(std::make_pair(5, std::any()));
	vec.push_back(std::make_pair(6, std::any()));
	vec.push_back(std::make_pair(4, std::any()));
	asd->read(vec.begin(), vec.end());*/
	return 0;
}