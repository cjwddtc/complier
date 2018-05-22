//单独抽象出来的一个类
//因为lr(1)和nfa转dfa中有着相同的操作(闭包和状态转移),所以将其抽象出来
#pragma once
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>

//实现hash函数，不重要
namespace std
{
	template<class state_type> 
	struct hash<std::unordered_set<state_type>>
	{
		typedef std::unordered_set<state_type> argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			result_type re = 0;
			std::hash<state_type> h;
			for (auto a : s)
			{
				re ^= h(a);
			}
			return re;
		}
	};
}
//这个通过预定义的闭包和状态转移操作实现nfa转dfa或者是lr(1)
template <class state_type,class input_type>
class state_to_map
{
public:
    typedef std::set<state_type> state_set;
	typedef std::map<input_type, state_set> next_map;
private:
public:
	//状态id对应的状态集合
    std::map<state_set,size_t> all;
	size_t n;
	//待next的状态
	std::set<const state_set*> read_to_next;
protected:
	//闭包将一个状态闭包为状态集合
    virtual void expand(state_type type,state_set &ptr)=0;
	//状态转移，求当前状态所能转移到的状态
    virtual void next(const state_set &type,next_map &map)=0;
	//当两个状态又转移关系时调用该函数
    virtual void link(const state_set &from_set,const state_set &to_set,input_type input)=0;
	//新建状态
    virtual void add_state(size_t n,const state_set &ptr)=0;
    size_t get_id(const state_set &ss)
    {
        return all[ss];
    }
	///***
	//插入新状态
	inline const state_set& insert(state_set &state)
	{
		state_set estate=state;
		for (state_type b : state)
		{
			expand(b, estate);
		}
		auto it = all.find(estate);
		if (it == all.end())
		{
			const state_set &s = all.emplace(std::move(estate), n).first->first;
			add_state(n, s);
			read_to_next.emplace(&s);
			++n;
			return s;
		}
		else {
			int i = 0;
		}
		return it->first;
	}
	///***
	//通过一个开始状态计算整个状态转移表
    void make_map(state_type start_state)
    {
		n = 0;
		read_to_next.clear();
		all.clear();
		state_set start_set;
		start_set.insert(start_state);
		insert(start_set);
        while(!read_to_next.empty())
        {
            const state_set &b=**read_to_next.begin();
            read_to_next.erase(read_to_next.begin());
            next_map map;
            next(b,map);
            for(auto a:map)
            {
				const state_set &next=insert(a.second);
                link(b,next,a.first);
            }
        }
    }
};
