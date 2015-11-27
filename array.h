#ifndef ARRAY_H
#define ARRAY_H
#include <memory>
#include <stddef.h>

template <class T>
class array
{
public:
    T *ptr;
    size_t max_size;
    array(size_t max_size_):ptr(new T[max_size_]),max_size(max_size_)
    {
    }
    array(const array<T> &other):max_size(other.max_size)
    {
        std::allocator<T> a;
        ptr=a.allocate(max_size);
        for(size_t i=0; i<max_size; i++)
        {
            a.construct(ptr+i,other.ptr[i]);
        }
    }
    array(array<T> &&other):ptr(other.ptr),max_size(other.max_size)
    {
        other.ptr=0;
    }
    ~array()
    {
        if(ptr!=0) delete[] ptr;
    }
    T &operator [](size_t n)
    {
        return ptr[n];
    }
};
#endif
