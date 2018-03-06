#include "yufashu.hpp"
#include <istream>
#include <iostream>
#include <algorithm>

project::project(input_type id_,const specification_left &to_id_, input_type fin_id_,size_t pos_):
    from_id(id_),to_id(to_id_),fin_id(fin_id_),pos(pos_) {}


project project::next() const
{
    return project(from_id,to_id,fin_id,pos+1);
}

bool project::operator==(const project &a)const
{
	return &to_id == &a.to_id&&fin_id == a.fin_id&&pos == a.pos;
}

namespace std
{
	hash<project>::result_type hash<project>::operator()(argument_type const& s) const noexcept
	{
		result_type const h2(std::hash<size_t>{}(s.fin_id));
		result_type const h3(std::hash<const char*>{}((const char *)&s.to_id));
		result_type const h4(std::hash<size_t>{}(s.pos));
		return h2^h3^h4;
	}
	hash<state_index>::result_type hash<state_index>::operator()(argument_type const& s) const noexcept
	{
		result_type const h1(std::hash<size_t>{}(s.first));
		result_type const h2(std::hash<size_t>{}(s.second));
		return h1 ^ h2;
	}
	hash<state_index>::result_type hash<symbol>::operator()(argument_type const& s) const noexcept
	{
		return result_type(std::hash<size_t>{}(s.id));
	}
}

grammer_impl::grammer_impl(state_map &&map_) :map(std::move(map_)){
	for (auto a : map)
	{
		printf("%lld,%lld\n",a.first.first,a.first.second);
	}
	printf("--%d", map.find(state_index(0, 1))==map.end());
	stack_state.push_back(0);
}

void grammer_impl::read_one(unit a)
{
	printf("%llu", a.first);
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
				if (arg.id != 0)
				{
					unit b(arg.id, std::any());
					arg.func(stack_symbol.end() - arg.size, stack_symbol.end(), b);
					stack_state.resize(stack_state.size() - arg.size);
					stack_symbol.resize(stack_symbol.size() - arg.size);
					//stack_state.erase(stack_state.end() - arg.size, stack_state.end());
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
}

void grammer_maker::get_first(input_type name,std::set<input_type> &set)
{
    if(gram_map.find(name)==gram_map.end())
    {
        set.insert(name);
    }
    else
    {
        auto itp=gram_map.equal_range(name);
        bool flag=true;
        for(auto it=itp.first; it!=itp.second; it++)
        {
			if (it->second.first.front() == name) {
				break;
			}
			else {
				get_first(it->second.first.front(), set);
			}
        }
    }
}

void grammer_maker::expand(state_type type,state_set &ptr)
{
    if(type.pos!=type.to_id.first.size())
    {
        input_type other=type.to_id.first[type.pos];
        auto itp=gram_map.equal_range(other);
        std::set<input_type> set;
        if(type.pos+1==type.to_id.first.size())
        {
            set.insert(type.fin_id);
		}
		else {
			get_first(type.to_id.first[type.pos + 1],set);
		}
        for(auto it=itp.first; it!=itp.second; it++)
        {
            for(auto id:set)
            {
                project prd(it->first,it->second,id);
                if(ptr.find(prd)==ptr.end())
                {
                    ptr.insert(prd);
                    expand(prd,ptr);
                }
            }
        }
    }
}

void grammer_maker::next(const state_set &ptr,next_map &map_)
{
    for(const state_type &a:ptr)
    {
        if(a.to_id.first.size()==a.pos)
        {
			specification s;
			s.id = a.from_id;
			s.size = a.to_id.first.size();
			s.func = a.to_id.second;
			map[std::make_pair(this->get_id(ptr),a.fin_id)] = s;
        }
        else
        {
            auto b=a.to_id.first[a.pos];
            map_[b].insert(a.next());
        }
    }
}

void grammer_maker::link(const state_set &from_set,const state_set &to_set,input_type input)
{
	shift s;
	s.state = this->get_id(to_set);
    map[std::make_pair(this->get_id(from_set),input)]=s;
}

void grammer_maker::add_state(size_t n,const state_set &ptr)
{
}
void grammer_maker::add(input_type a, std::vector<input_type> &&b, specification_handle handler)
{
	gram_map.emplace(a, std::make_pair(std::move(b),handler));
}
grammer_impl *grammer_maker::make_grammer(input_type root_state, specification_handle root_handle)
{
	auto b = gram_map.emplace(0, std::make_pair(std::vector<size_t>({ root_state }), root_handle));
	this->make_map(project(0,b->second,-1));
	gram_map.clear();
	return new grammer_impl(std::move(map));
}
thread_local grammer_maker global_grammer_impl;
thread_local size_t sid = 1;

int main_() {
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
	grammer asd = make_grammer(a, [](auto ...arg) {});
	std::vector<unit> vec;
	vec.push_back(std::make_pair(6, std::any()));
	vec.push_back(std::make_pair(5, std::any()));
	vec.push_back(std::make_pair(6, std::any()));
	vec.push_back(std::make_pair(4, std::any()));
	asd.read(vec.begin(), vec.end());
	return 0;
}

symbol::symbol():id(sid++)
{
}

binder symbol::operator=(std::initializer_list<symbol> symbols)
{
	std::vector<input_type> vec(symbols.size());
	std::transform(symbols.begin(), symbols.end(), vec.begin(), [](auto a) {return a.id; });
	return binder(new binder_impl(id,std::move(vec)));
}

bool symbol::operator==(const symbol &o) const
{
	return id==o.id;
}

binder_impl::binder_impl(input_type id_, std::vector<input_type>&& syms):id(id_), symbols(std::move(syms))
{
	
}

void binder_impl::operator,(specification_handle han)
{
	global_grammer_impl.add(id, std::move(symbols), han);
}

grammer make_grammer(symbol sym, specification_handle root_handle)
{
	sid = 1;
	return grammer(global_grammer_impl.make_grammer(sym.id,root_handle));
}


grammer::grammer(grammer_impl * p):_imp(p)
{

}
void grammer::read_one(unit a)
{
	_imp->read_one(a);
}

binder::binder(binder_impl * p):_imp(p)
{

}

void binder::operator,(specification_handle handle)
{
	(*_imp), handle;
}
//
namespace std {
	template class shared_ptr<binder_impl>;
	template class shared_ptr<grammer_impl>;
}