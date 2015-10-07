#ifndef MAP_TWO_HPP_INCLUDED
#define MAP_TWO_HPP_INCLUDED
#include <vector>
#include <utility>
template <class T>
constexpr int getspace(){return 1<<(sizeof(T)*8);}

template <class state_type,class char_type>
class state_line
{
public:
    state_type *ptr;
    state_line()
    {
        ptr=new state_type[getspace<char_type>()];
    }
    state_line(state_line<state_type,char_type> &&a)
    {
        ptr=a.ptr;
    }
    state_line(const state_line<state_type,char_type> &other){
        ptr=new state_type[getspace<char_type>()];
        memcpy(ptr,other.ptr,(1<<sizeof(char_type))*sizeof(state_type));
    }
    ~state_line()
    {
        delete[] ptr;
    }
    state_type &operator[](int n)
    {
        assert(n<getspace<char_type>());
        assert(n>=0);
        return ptr[n];
    }
};
// no use
template <class state_type,class char_type>
class node{
    state_type after[getspace<char_type>()];
    state_type before[getspace<char_type>()];
    node(){
        memset(after,0,getspace<char_type>()*sizeof(state_type));
        memset(before,0,getspace<char_type>()*sizeof(state_type));
    }
    node(const node<state_type,char_type> &a){
        memcpy(after,a.after,getspace<char_type>()*sizeof(state_type));
        memcpy(before,a.before,getspace<char_type>()*sizeof(state_type));
    }
};
//no use
template <class T,unsigned char N>
class multivector{
    size_t index[N];
    std::vector<T> vec[N];
    size_t add(T a,unsigned char id){
        vec[id].push_back(a);
        for(int i=id;i<N;i++){
            index[i]++;
        }
        return index[++id]--;
    }
    std::pair<size_t,unsigned char> analyse(size_t pos){
        for(int i=1;i<N;i++){
            if(pos<index[i]){
                i--;
                pos-=index[i];
                std::make_pair(vec[i][pos],i);
            }
        }
    }
};

template <class T>
class map_two
{
    std::vector<T> list;
    size_t div;
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
    int add_A()
    {
        plus.push_back(T());
        return plus.size();
    }
    int add_B()
    {
        minus.push_back(T());
        return -minus.size();
    }
};


#endif // MAP_TWO_HPP_INCLUDED
