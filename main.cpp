#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <assert.h>
#include "dfa.h"
#include "predef.h"
#include "cffx.hpp"
#include "regex_map_two.hpp"
using std::string;
using std::cout;
using std::endl;

nfa<char> as;

int main()
{
    auto_machin<int,char> b;
    string str="a+(s*|(d-a))?|fi";
    auto sta=str.begin();
    state_map<int,char> a;
    int n=0;
    regex_node_block<state_map<int,char>> *q=read_regex_string<state_map<int,char>>(sta,str.end());
    for(auto b:q->regex_nodes){
        b->write_to_map(a,n);
    }
    std::set<nfa_state<char>*> asdq;
    dfa<char,int> qwe(as);
    return 0;
}
