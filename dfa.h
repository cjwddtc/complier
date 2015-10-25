#ifndef DFA_H_INCLUDED
#define DFA_H_INCLUDED
#include <map>
#include <set>
#include <vector>
#include <assert.h>
#include "map_two.hpp"
#include <iostream>

template <class type>
size_t get_range()
{
    assert(1);
    return 0;
}

template <class char_type>
class nfa_state:public std::multimap<char_type,nfa_state<char_type> *>
{
public:
    std::string name;
    nfa_state(std::string name_=""):name(name_){}
    void link(char_type ch,nfa_state<char_type> state)
    {
        insert(make_pair(ch,state));
    }
};
template <class char_type>
using dfa_state=std::set<nfa_state<char_type>*>;

template <class char_type>
class nfa
{
public:
    std::set<nfa_state<char_type>*> start_state;
    std::map<nfa_state<char_type>*,std::string> fin_state;
    static std::set<nfa_state<char_type>*> move(std::set<nfa_state<char_type>*> source,char_type ch)
    {
        std::set<nfa_state<char_type>*> ret_set;
        for(nfa_state<char_type>* node:source)
        {
            auto b=node->find(ch);
            while(b!=node->end())
            {
                ret_set.insert(b->second);
            }
        }
        return ret_set;
    }
};

template <class char_type,class state_type>
class dfa:public std::vector<state_line<state_type,char_type>>
{
public:

    typedef state_type type_state;
    typedef char_type type_code;
    //¿É½áÊø×´Ì¬±í
    std::map<state_type,std::string> finish_map;
    dfa(const nfa<char_type> &n)
    {
        //all state
        std::map<dfa_state<char_type>,state_type> a;
        //have state id ready to next
        std::map<dfa_state<char_type>,state_type> b;
        b.insert(make_pair(n.start_state,0));
        a.insert(make_pair(n.start_state,0));
        while(!b.empty())
        {
            for(char_type ch=0;ch<get_space<char_type>();ch++){
                dfa_state<char_type> c=nfa<char_type>::move(b.begin()->first,ch);
                if(a.find(c)==a.end())
                {
                    size_t n=std::vector<state_line<state_type,char_type>,state_type>::size();
                    b.insert(make_pair(c,n));
                    a.insert(make_pair(c,n));
                    at(a[c])[ch]=n;
                    push_back(state_line<state_type,char_type>());
                    for(nfa_state<char_type>* d:c){
                        if(d->name!=""){
                            finish_map[n]=d->name;
                            break;
                        }
                    }
                }
            }
            b.erase(b.begin());
        }
    }
    void write_to_file(std::ostream file){
        size_t n=std::vector<state_line<state_type,char_type>,state_type>::size();
        file.write((const char *)&n,sizeof(n));
        for(state_line<state_type,char_type> line:(*this)){
            file.write(line.ptr,get_space<char_type>());
        }
        for(auto it=finish_map.begin();it!=finish_map.end();it++){

        }
    }
};


#endif // DFA_H_INCLUDED
