#ifndef MAP_TWO_HPP_INCLUDED
#define MAP_TWO_HPP_INCLUDED
#include <vector>
#include <utility>
template <class T>
constexpr int get_space()
{
    return 1<<(sizeof(T)*8);
}

template <class state_type,class char_type>
class state_line
{
public:
    state_type *ptr;
    bool is_last;
    state_line(bool is_last_):is_last(is_last_)
    {
        ptr=new state_type[get_space<char_type>()];
    }
    state_line(state_line<state_type,char_type> &&a)
    {
        ptr=a.ptr;
    }
    state_line(const state_line<state_type,char_type> &other)
    {
        ptr=new state_type[get_space<char_type>()];
        memcpy(ptr,other.ptr,(1<<sizeof(char_type))*sizeof(state_type));
    }
    ~state_line()
    {
        delete[] ptr;
    }
    state_type &operator[](int n)
    {
        assert(n<get_space<char_type>());
        assert(n>=0);
        return ptr[n];
    }
};

// no use
template <class state_type,class char_type>
class node
{
    state_type after[get_space<char_type>()];
    state_type before[get_space<char_type>()];
    node()
    {
        memset(after,0,get_space<char_type>()*sizeof(state_type));
        memset(before,0,get_space<char_type>()*sizeof(state_type));
    }
    node(const node<state_type,char_type> &a)
    {
        memcpy(after,a.after,get_space<char_type>()*sizeof(state_type));
        memcpy(before,a.before,get_space<char_type>()*sizeof(state_type));
    }
};
//no use
template <class T,unsigned char N>
class multivector
{
    size_t index[N];
    std::vector<T> vec[N];
    size_t add(T a,unsigned char id)
    {
        vec[id].push_back(a);
        for(int i=id; i<N; i++)
        {
            index[i]++;
        }
        return index[++id]--;
    }
    std::pair<size_t,unsigned char> analyse(size_t pos)
    {
        for(int i=1; i<N; i++)
        {
            if(pos<index[i])
            {
                i--;
                pos-=index[i];
                std::make_pair(vec[i][pos],i);
            }
        }
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
