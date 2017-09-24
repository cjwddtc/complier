#ifndef TEMPLATE_HPP_INCLUDED
#define TEMPLATE_HPP_INCLUDED
#include <memory>
#include <set>
#include <map>
#include "array.h"
//#define DEBUG_MAP

template <class T>
struct ptr_less
{
    bool operator()(const T a,const T b)const
    {
        return *a<*b;
    }
};

template <class state_type,class input_type>
class state_to_map
{
public:
    typedef std::set<state_type> state_set;
    typedef std::shared_ptr<state_set> state_set_ptr;
    typedef std::map<input_type,state_set_ptr> next_map;
private:
    std::map<state_set_ptr,size_t,ptr_less<state_set_ptr>> all;
protected:
    virtual void expand(state_type type,state_set_ptr ptr)=0;
    virtual void next(state_set_ptr ptr,next_map &map)=0;
    virtual void link(state_set_ptr from_set,state_set_ptr to_set,input_type input)=0;
    virtual void add_state(size_t n,state_set_ptr ptr)=0;
    virtual state_set_ptr expand(state_set_ptr ptr)
    {
        state_set_ptr pt=std::make_shared<state_set>(*ptr);
        for(state_type b:*ptr)
        {
            expand(b,pt);
        }
        return pt;
    }
    virtual size_t get_id(state_set_ptr ss)
    {
        return all[ss];
    }
    virtual void make_map(state_type state)
    {
        state_set_ptr qwe=std::make_shared<state_set>();
        qwe->insert(state);
        state_set_ptr ptr=expand(qwe);
        std::set<state_set_ptr,ptr_less<state_set_ptr>> read_to_next;
        size_t n=0;
        add_state(n,ptr);
        all.insert(std::make_pair(ptr,n++));
        read_to_next.insert(ptr);
        while(!read_to_next.empty())
        {
            auto b=*read_to_next.begin();
            read_to_next.erase(read_to_next.begin());
            next_map map;
#ifdef DEBUG_MAP
            printf("from:%d-----------------\n",get_id(b));
            for(auto a:*b)
            {
                a.print();
            }
            printf("----------------------\n");
#endif
            next(b,map);
            for(auto a:map)
            {
#ifdef DEBUG_MAP
                printf("before:%d---------------\n",a.first);
                for(auto b:*a.second)
                {
                    b.print();
                }
                printf("----------------------\n");
#endif
                state_set_ptr ptr=expand(a.second);
#ifdef DEBUG_MAP
                printf("after:-----------------\n");
                for(auto b:*ptr)
                {
                    b.print();
                }
                printf("----------------------\n");
#endif
                if(all.find(ptr)==all.end())
                {
                    add_state(n,ptr);
                    all.insert(std::make_pair(ptr,n++));
                    read_to_next.insert(ptr);
                }
                link(b,ptr,a.first);
            }
        }
    }
}
;


#endif // TEMPLATE_HPP_INCLUDED
