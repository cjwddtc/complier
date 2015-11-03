#ifndef CFFX_HPP_INCLUDED
#define CFFX_HPP_INCLUDED
#include <iostream>
#include "map_two.hpp"

using std::basic_string;
template <class state_type,class code_type>
class print_
{
public:
    void operator()(std::basic_string<code_type> a,state_type b)
    {
        std::cout << a << "\n" << b << std::endl;
    }
};
 
template<class state_type,class code_type,class T=print_<state_type,code_type>>
class auto_machin
{
public:
    state_map<state_type,code_type> map;
    state_type currentstate;
    T out;
    basic_string<code_type> buf;
    template<class ...Arg>
    auto_machin(Arg ...args):out(args...)
    {
    }
    state_type readone(code_type a)
    {
        buf.push_back(a);
        state_type newstate=map[a][currentstate];
        if(newstate==0)
        {
            if(currentstate>0)
            {
                T(buf,currentstate);
            }
            else
            {
                throw a;
            }
        }
        return currentstate=newstate;
    }
};


#endif // CFFX_HPP_INCLUDED
