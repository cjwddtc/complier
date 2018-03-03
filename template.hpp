#pragma once
#include <set>
#include <map>

template <class state_type,class input_type>
class state_to_map
{
public:
    typedef std::set<state_type> state_set;
    typedef std::map<input_type,state_set> next_map;
private:
    std::map<state_set,size_t> all;
	size_t n;
	std::set<state_set&> read_to_next;
protected:
    virtual void expand(state_type type,state_set &ptr)=0;
    virtual void next(state_type type,next_map &map)=0;
    virtual void link(state_set &from_set,state_set &to_set,input_type input)=0;
    virtual void add_state(size_t n,state_set *ptr)=0;
    size_t get_id(state_set &ss)
    {
        return all[ss];
    }
	inline state_set& insert(state_set &state)
	{
		state_set estate;
		for (state_type b : state)
		{
			expand(b, estate);
		}
		auto it = all.find(estate);
		if (it == all.end())
		{
			state_set &s = all.emplace(std::move(estate), n).second->first;
			add_state(n, s);
			read_to_next.emplace(s);
			++n;
			return s;
		}
		return it->first;
	}
    void make_map(state_type start_state)
    {
		insert(start_state);
        while(!read_to_next.empty())
        {
            state_set &b=read_to_next.begin();
            read_to_next.erase(read_to_next.begin());
            next_map map;
            next(b,map);
            for(auto a:map)
            {
				state_set &next=insert(a.second);
                link(b,next,a.first);
            }
        }
    }
};
