#ifndef TEMPLATE_HPP_INCLUDED
#define TEMPLATE_HPP_INCLUDED
#include <set>
#include <map>
#include "array.h"


template <class T>
struct ptr_less{
    bool operator()(const T *a,const T *b){
        return *a<*b;
    }
};

template <class state_type,class input_type>
class state_to_map
{
protected:
    typedef std::set<state_type> state_set;
    typedef std::map<input_type,state_set> next_map;
    std::map<state_set *,size_t,ptr_less<state_set>> all;
    virtual void expand(state_type type,state_set *ptr)=0;
    virtual void next(state_set *ptr,next_map &map)=0;
    virtual void link(state_set *from_set,state_set *to_set,input_type input)=0;
    virtual void add_state(size_t n,state_set *ptr)=0;
    virtual state_set *expand(state_set *ptr)
    {
        state_set *pt=new state_set(*ptr);
        for(state_type b:*ptr){
            expand(b,pt);
        }
        return pt;
    }
    virtual size_t get_id(state_set *ss){
        return all[ss];
    }
    virtual void make_map(state_type state)
    {
        state_set qwe;
        qwe.insert(state);
        state_set *ptr=expand(&qwe);
        std::set<state_set *,ptr_less<state_set>> read_to_next;
        size_t n=0;
        add_state(n,ptr);
        all.insert(std::make_pair(ptr,n++));
        read_to_next.insert(ptr);
        while(!read_to_next.empty())
        {
            auto b=*read_to_next.begin();
            read_to_next.erase(read_to_next.begin());
            next_map map;
            next(b,map);
            for(auto a:map){
                state_set *ptr=expand(&a.second);
                if(all.find(ptr)==all.end()){
                    add_state(n,ptr);
                    all.insert(std::make_pair(ptr,n++));
                    read_to_next.insert(ptr);
                }
                else
                    delete ptr;
                link(b,ptr,a.first);
            }
        }
        for(auto a:all){
            delete a.first;
        }
    }
}
;


#endif // TEMPLATE_HPP_INCLUDED
