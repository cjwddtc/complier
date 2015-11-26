#ifndef DFA_H_INCLUDED
#define DFA_H_INCLUDED
#include <map>
#include "predef.h"
#include "map_two.hpp"
#include "template.hpp"
#include "gram_tree.h"
#include <set>
#include <vector>
#include <assert.h>
#include <memory>
#include <iostream>
#include <iterator>
//#define DEBUG_MAP

template <class char_type>
class nfa_state;

template <class char_type>
class nfa
{
public:
    typedef char_type type_char;
    nfa_state<char_type>* start_state;
    std::map<nfa_state<char_type>*,std::string> fin_state;
    nfa();
    void print();
};

template <class char_type>
class nfa_state
{
public:
    std::multimap<char_type,nfa_state<char_type> *> edge;
    typedef char_type type_char;
    std::string name;
    nfa_state(std::string name_=""):name(name_) {}
    void link(char_type ch,nfa_state<char_type> *state)
    {
        edge.insert(std::make_pair(ch,state));
    }
    void print(std::set<nfa_state<char_type>*> *a)
    {
        //printf("%p\n",this);
        if(a->find(this)==a->end())
        {
            a->insert(this);
            for(auto b:edge)
            {
                printf("%p\t%c\t%p\n",this,b.first,b.second);
                b.second->print(a);
            }
        }
    }
};
template <class char_type>
using dfa_state=std::set<nfa_state<char_type>*>;

template <class char_type>
nfa<char_type>::nfa()
{
    start_state=new nfa_state<char_type>();
}


template <class char_type>
void nfa<char_type>::print()
{
    std::set<nfa_state<char_type>*> a;
    start_state->print(&a);
}

template <class char_type>
class dfa:public state_to_map<nfa_state<char_type>*,char_type>
{
public:
    typedef char_type type_code;
    typedef typename state_to_map<nfa_state<char_type>*,char_type>::state_set state_set;
    typedef typename state_to_map<nfa_state<char_type>*,char_type>::next_map next_map;
    size_t current_state;
    std::string result;
    //finish state map
    std::map<size_t,std::string> finish_map;
    std::vector<state_line<size_t,char_type>> state_map;
    virtual void expand(nfa_state<char_type> *one,state_set *ptr)
    {
        auto b=one->edge.find(get_null_value<char_type>());
        while(b!=one->edge.end() && b->first==get_null_value<char_type>())
        {
            if(ptr->find(b->second)==ptr->end())
            {
                ptr->insert(b->second);
                expand(b->second,ptr);
            }
            ++b;
        }
    }
    virtual void next(state_set *ptr,next_map &map)
    {
        for(nfa_state<char_type> *b:*ptr)
        {
            for(auto a:b->edge)
            {
                map[a.first].insert(a.second);
            }
        }
    }
    virtual void link(state_set *from_set,state_set *to_set,char_type ch)
    {
        state_map[this->get_id(from_set)][ch]=this->get_id(to_set);
    }
    virtual void add_state(size_t n,state_set *ptr)
    {
        if(state_map.size()!=n)
        {
            throw n;
        }
        state_map.push_back(state_line<size_t,char_type>());
        for(nfa_state<char_type>* d:*ptr)
        {
            if(d->name=="id" && finish_map[n]=="")
            {
                finish_map[n]=d->name;
            }else if(d->name!="")
            {
                finish_map[n]=d->name;
                break;
            }
        }
    }
    dfa(const nfa<char_type> &n)
    {
        current_state=0;
        make_map(n.start_state);
    }
    dfa(std::istream &file)
    {
        current_state=0;
        size_t *ptr=new size_t[get_range_hight<char_type>()-get_range_low<char_type>()];
        size_t n;
        file.read((char *)&n,sizeof(n));
        for(size_t i=0; i<n; i++)
        {
            file.read((char *)ptr,get_range_hight<char_type>()-get_range_low<char_type>());
            //for()
            state_map.push_back(state_line<size_t,char_type>(ptr));
        }
        size_t m;
        while(!file.eof())
        {
            file.read((char *)&m,sizeof(m));
            std::string str;
            char ch;
            while(1)
            {
                file.get(ch);
                if(ch==0)
                    break;
                str.push_back(ch);
            }
            finish_map[m]=str;
        }
    }
    template <class ITERATOR,class FUNC>
    void read(ITERATOR start,ITERATOR end,FUNC fun)
    {
        std::string a;
        int line=1;
        while(start!=end)
        {
            if(read(*start))
            {
                if(result!="null")
                {
                    fun(gram_tree_node(result,a));
                }
                a.clear();
                if(*start=='\n')
                {
                    line++;
                }
                result="";
            }
            else
            {
                a.push_back(*start++);
            }
        }
        auto q=finish_map.find(current_state);
        if(q!=finish_map.end())
        {
            result=q->second;
            fun(gram_tree_node(result,a));
        }
    }
    void write_to_file(std::ostream &file)
    {
        size_t n=state_map.size();
        file.write((const char *)&n,sizeof(n));
        for(state_line<size_t,char_type> line:state_map)
        {
            file.write((const char *)line.ptr.data(),get_range_hight<char_type>()-get_range_low<char_type>());
        }
        for(auto it=finish_map.begin(); it!=finish_map.end(); it++)
        {
            file.write((const char *)&it->first,sizeof(it->first));
            file.write(it->second.c_str(),it->second.size()+1);
        }
    }
    bool read(char_type ch)
    {
        auto a=state_map[current_state][ch];
        if(a==0)
        {
            auto a=finish_map.find(current_state);
            if(a!=finish_map.end())
            {
                result=a->second;
            }
            else
            {
                throw a;
            }
        }
        current_state=a;
        return a==0;
    }
};


#endif // DFA_H_INCLUDED
