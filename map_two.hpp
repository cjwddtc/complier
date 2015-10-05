#ifndef MAP_TWO_HPP_INCLUDED
#define MAP_TWO_HPP_INCLUDED
#include <vector>

template <class state_type,class char_type>
class state_line
{
public:
    state_type *ptr;
    state_line()
    {
        ptr=new state_type[1<<sizeof(char_type)];
    }
    state_line(state_line<state_type,char_type> &&a)
    {
        ptr=a.ptr;
    }
    ~state_line()
    {
        delete[] ptr;
    }
    state_type &operator[](int n)
    {
        assert(n<(1<<sizeof(char_type)));
        assert(n>=0);
        return ptr[n];
    }
};

template <class T>
class map_two
{

    std::vector<T> plus;
    std::vector<T> minus;
public:
    map_two()
    {
        plus.resize(1);
        minus.resize(1);
    }
    T &operator[](int n)
    {
        if(n>0)
        {
            return plus[n];
        }
        else if(n<0)
        {
            return minus[-n];
        }
        else
        {
            assert(1);
        }
    }
    int add_plus()
    {
        plus.push_back(T());
        return plus.size();
    }
    int add_minus()
    {
        minus.push_back(T());
        return -minus.size();
    }
};


#endif // MAP_TWO_HPP_INCLUDED
