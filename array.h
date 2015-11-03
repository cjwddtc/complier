#include <allocator>


template <class T>
class array{
    T *ptr;
    size_t max_size;
    array(size_t max_size_):max_size(max_size_),ptr(new T[max_size]){
    }
    array(const array<T> &other):max_size(other.max_size){
        allocator<T> a;
        ptr=a.allocate(max_size);
        for(size_t i=0;i<max_size;i++){
            a.construct(ptr+i,other.ptr[i]);
        }
    }
    array(array<T> &&other):max_size(other.max_size),ptr(other.ptr){}
    ~array(){
        delete[] ptr;
    }
    T &operator [](size_t n){
        return ptr[n];
    }
}
