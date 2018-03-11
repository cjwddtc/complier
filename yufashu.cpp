#include <iostream>
#include <algorithm>
#include "yufashu.hpp"
using namespace yacc;
//lr(1)项目
class project
{
public:
	//产生式左端
	input_type from_id;
	//产生式右端
	const specification_left &to_id;
	//规约条件（后继符号）
	input_type fin_id;
	//当前位置
	size_t pos;
	project(input_type id_, const specification_left &to_id_, input_type fin_id_, size_t pos_ = 0) :
		from_id(id_), to_id(to_id_), fin_id(fin_id_), pos(pos_) {}
	bool operator==(const project &a)const
	{
		return &to_id == &a.to_id&&fin_id == a.fin_id&&pos == a.pos;
	}
	//返回向后移动了一格的项目
	project next() const
	{
		return project(from_id, to_id, fin_id, pos + 1);
	}
	//void print();
};
//hash函数不重要
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

gammer::gammer(state_map &&map_) :map(std::move(map_)){}

void gammer::read_one(unit a)
{
	auto it = map.find(state_index(stack_state.back(), a.first));
	if (it != map.end())
	{
		std::visit([this, a](auto arg) {
			using T = std::decay_t<decltype(arg)>;
			//如果是移进
			if constexpr (std::is_same_v<T, shift>)
			{
				stack_symbol.emplace_back(a.second);
				stack_state.emplace_back(arg.state);
			}
			//如果是规约
			else if constexpr (std::is_same_v<T, specification>)
			{
				//调用对应的规约函数
				any b = arg.func(stack_symbol.end() - arg.size);
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
					read_one(unit(arg.id,b));
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
//将一组产生式转发为语法分析器，state_to_map详见template.hpp
class grammer_maker :public state_to_map<state_type, input_type>
{
	typedef std::unordered_map<state_index, op> state_map;
public:
	typedef typename state_to_map<state_type, input_type>::state_set state_set;
	typedef typename state_to_map<state_type, input_type>::next_map next_map;
	std::unordered_multimap<input_type, specification_left> gram_map;
	state_map map;
	grammer_maker() = default;
	///***
	//求first集
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
					continue;
				}
				else {
					get_first(it->second.first.front(), set);
				}
			}
		}
	}
	///***
	//闭包
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
	///***
	//状态转移
	virtual void next(const state_set &ptr, next_map &map_)
	{
		for (const state_type &a : ptr)
		{
			if (a.to_id.first.size() == a.pos)
			{
				//如果移动到了末尾规约
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
	///***
	//移进
	virtual void link(const state_set &from_set, const state_set &to_set, input_type input)
	{
		shift s;
		s.state = this->get_id(to_set);
		map[std::make_pair(this->get_id(from_set), input)] = s;
	}

	virtual void add_state(size_t n, const state_set &ptr){}
	///***
	//添加一个产生式并用handle作为规约函数
	void add(input_type a, std::vector<input_type> &&b, specification_handle handler)
	{
		gram_map.emplace(a, std::make_pair(std::move(b), handler));
	}
	///***
	//生成语法分析器
	std::shared_ptr<gammer> make_grammer(input_type root_state, specification_handle root_handle)
	{
		auto b = gram_map.emplace(0, std::make_pair(std::vector<size_t>({ root_state }), root_handle));
		this->make_map(project(0, b->second, -1));
		gram_map.clear();
		return std::make_shared<gammer>(std::move(map));
	}
	//void read(gram_tree_node b);
};

//下面都不重要
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

void yacc::binder::operator,(pass_by p)
{
	if (p.n == -1) {
		add([](unit_it b) {return not_use(); });
	}
	else {
		add([a = p.n](unit_it b){return b[a]; });
	}
}


void binder::add(specification_handle han)
{
	global_grammer_impl.add(id, std::move(symbols), han);
}

std::shared_ptr<yacc::gammer> yacc::make_grammer(symbol sym, std::function<void(not_use)> root_handle)
{
	return make_grammer(sym,(yacc::specification_handle) [](unit_it a) {return not_use(); });
}
//一直到这里位置都不重要
//原来测试用的现在当做例子
//详细用法请看main.cpp
int main_() {
	//语法分析器用法
	//声明文法符号
	symbol a;
	symbol b;
	symbol c;
	symbol d;

	//定义产生式后面的是lambda表达式相当于动态定义的函数，产生式规约时将会被调用
	//为什么可以写成这样，是因为我重载了赋值运算符和逗号运算符
	a = { a,b }, [](not_use) {
		printf("ab\n");
		return not_use();
	};
	a = { c,a }, [](not_use) {
		printf("ca\n");
		return not_use();
	};
	a = { d }, [](not_use) {
		printf("d\n");
		return not_use();
	};
	//生成语法分析器
	std::shared_ptr<gammer> asd = make_grammer(a, [](not_use a) { });
	std::vector<unit> vec;
	vec.push_back(std::make_pair(c.id, std::any(1)));
	vec.push_back(std::make_pair(d.id, std::any(1)));
	vec.push_back(std::make_pair(b.id, std::any(1)));
	//用这个数组作为输入输出d\nca\nab\n
	asd->read(vec.begin(), vec.end());
	return 0;
}

yacc::pass_by::pass_by(size_t m):n(m)
{
}
