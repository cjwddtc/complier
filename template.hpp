#pragma once
#include <unordered_set>
#include <unordered_map>


//ÊµÏÖhashº¯Êı
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

template <class state_type,class input_type>
class state_to_map
{
public:
    typedef std::unordered_set<state_type> state_set;
	typedef std::unordered_map<input_type, state_set> next_map;
private:
    std::unordered_map<state_set,size_t> all;
	size_t n;
	std::unordered_set<const state_set*> read_to_next;
protected:
    virtual void expand(state_type type,state_set &ptr)=0;
    virtual void next(const state_set &type,next_map &map)=0;
    virtual void link(const state_set &from_set,const state_set &to_set,input_type input)=0;
    virtual void add_state(size_t n,const state_set &ptr)=0;
    size_t get_id(const state_set &ss)
    {
        return all[ss];
    }
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
    void make_map(state_type start_state)
    {
		n = 0;
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
