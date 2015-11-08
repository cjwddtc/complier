#ifndef DFA_H_INCLUDED
#define DFA_H_INCLUDED
#include <map>
#include "predef.h"
#include "map_two.hpp"
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
using state_set=std::set<nfa_state<char_type>*>;
template <class char_type>
using ptr_state_set=std::unique_ptr<state_set<char_type>>;
template <class char_type>
class nfa
{
public:
    typedef char_type type_char;
    std::set<nfa_state<char_type>*> start_state;
    std::map<nfa_state<char_type>*,std::string> fin_state;
    nfa();
    static state_set<char_type> move(state_set<char_type> source,char_type ch);
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
    void expand(state_set<char_type> *source)
    {
        auto b=edge.find(get_null_value<char_type>());
        while(b!=edge.end() && b->first==get_null_value<char_type>())
        {
            if(source->find(b->second)==source->end())
            {
                source->insert(b->second);
                b->second->expand(source);
            }
            ++b;
        }
    }
};
template <class char_type>
using dfa_state=std::set<nfa_state<char_type>*>;

template <class char_type>
nfa<char_type>::nfa()
{
    start_state.insert(new nfa_state<char_type>());
}

template <class char_type>
state_set<char_type> nfa<char_type>::move(state_set<char_type> source,char_type ch)
{
    state_set<char_type> ret_set;
    for(nfa_state<char_type>* node:source)
    {
        auto b=node->edge.find(ch);
        while(b!=node->edge.end() && b->first==ch)
        {
            ptr_state_set<char_type> exp_set(new state_set<char_type>());
            ret_set.insert(b->second);
            b->second->expand(exp_set.get());
            ret_set.insert(exp_set->begin(),exp_set->end());
            b++;
        }
    }/*
    if(!ret_set.empty()){
    printf("---------------------------\n");
    for(auto b:ret_set){
        printf("%p\t",b);
    }
    printf("---------------------------\n");
    }*/
    return ret_set;
}

template <class char_type>
void nfa<char_type>::print()
{
    std::set<nfa_state<char_type>*> a;
    (*start_state.begin())->print(&a);
}

template <class char_type,class state_type>
class dfa
{
public:

    typedef state_type type_state;
    typedef char_type type_code;
    state_type current_state;
    std::string result;
    //finish state map
    std::map<state_type,std::string> finish_map;
    std::vector<state_line<state_type,char_type>> state_map;
    dfa(const nfa<char_type> &n)
    {
        current_state=0;
        ptr_state_set<char_type> fb(new state_set<char_type>);
        for(nfa_state<char_type> *a:n.start_state)
        {
            a->expand(fb.get());
            fb->insert(a);
        }
        //all state
        std::map<dfa_state<char_type>,state_type> a;
        //have state id ready to next
        std::map<dfa_state<char_type>,state_type> b;
        b.insert(make_pair(*fb,0));
        a.insert(make_pair(*fb,0));
        state_map.push_back(state_line<state_type,char_type>());
        state_type current;
        while(!b.empty())
        {
            state_type sta=b.begin()->second;
            dfa_state<char_type> nset(b.begin()->first);
            b.erase(b.begin());
            for(char_type ch=get_range_low<char_type>(); ch<get_range_hight<char_type>(); ch++)
            {
                if(ch==get_null_value<char_type>())
                {
                    continue;
                }
                dfa_state<char_type> c=nfa<char_type>::move(nset,ch);
                if(c.empty()) continue;
                if(a.find(c)==a.end())
                {
                    int n=state_map.size();
                    b.insert(make_pair(c,n));
                    a.insert(make_pair(c,n));
                    state_map.push_back(state_line<state_type,char_type>());
                    state_map[sta][ch]=n;
                    for(nfa_state<char_type>* d:c)
                    {
                        if(d->name!="")
                        {
                            finish_map[n]=d->name;
                            //printf("name:%d\t%s\n",n,d->name.c_str());
                            break;
                        }
                    }
#ifdef DEBUG_MAP
                    printf("add\t%d\t%d\t%d\n",sta,ch,state_map[sta][ch]);
#endif
                }
                else
                {
                    state_map[sta][ch]=a[c];
#ifdef DEBUG_LMAP
                    printf("link\t%d\t%d\t%d\n",sta,ch,state_map[sta][ch]);
#endif
                }

            }
        }
    }
    dfa(std::istream &file)
    {
        current_state=0;
        state_type *ptr=new state_type[get_range_hight<char_type>()-get_range_low<char_type>()];
        size_t n;
        file.read((char *)&n,sizeof(n));
        for(size_t i=0; i<n; i++)
        {
            file.read((char *)ptr,get_range_hight<char_type>()-get_range_low<char_type>());
            //for()
            state_map.push_back(state_line<state_type,char_type>(ptr));
        }
        state_type m;
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
                    fun(make_pair(result,a));
                }
                a.clear();
                if(*start=='\n')
                {
                    line++;
                }
            }
            else
            {
                a.push_back(*start++);
            }
        }
    }
    void write_to_file(std::ostream &file)
    {
        size_t n=state_map.size();
        file.write((const char *)&n,sizeof(n));
        for(state_line<state_type,char_type> line:state_map)
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
