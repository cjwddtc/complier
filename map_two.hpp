#ifndef MAP_TWO_HPP_INCLUDED
#define MAP_TWO_HPP_INCLUDED
#include <vector>
#include <utility>
#include <limits.h>
#include "predef.h"

template <class state_type,class char_type>
class state_line
{
public:
    std::vector<state_type> ptr;
    state_line():ptr()
    {
        ptr.resize(get_range_hight<char_type>()-get_range_low<char_type>());
    }
    state_line(state_type *optr)
    {
        ptr.assign(optr,optr+get_range_hight<char_type>()-get_range_low<char_type>());
    }
    state_line(state_line<state_type,char_type> &&a):ptr(std::move(a.ptr))
    {
    }
    state_line(const state_line<state_type,char_type> &other):ptr(other.ptr) {}
    ~state_line()
    {
        ;
    }
    state_type &operator[](char_type ch)
    {
        return ptr[ch-get_range_low<char_type>()];
    }
};

template <class T,class index_tpye>
class map_two
{
    std::vector<T> A;
    std::vector<T> B;
public:
    typedef T value_type;
    map_two()=default;
    //two array ,one plus one minus
    T &operator[](index_tpye n)
    {
        //don't accept 0
        assert(n);
        if(n>0)
        {
            return A[n-1];
        }
        else
        {
            return B[-1-n];
        }
    }
    index_tpye add_A(T &&a)
    {
        A.push_back(std::move(a));
        return A.size();
    }
    index_tpye add_B(T &&a)
    {
        B.push_back(std::move(a));
        return -B.size();
    }
    index_tpye add_A(const T &a)
    {
        A.push_back(a);
        return A.size();
    }
    index_tpye add_B(const T &a)
    {
        B.push_back(a);
        return -B.size();
    }
};
template<class state_type,class code_type>
class state_map:public map_two<state_line<state_type,code_type>,state_type>
{
public:
    typedef state_type type_state;
    typedef code_type type_code;
};


#endif // MAP_TWO_HPP_INCLUDED
