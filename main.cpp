#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <fstream>
#include <assert.h>
#include "predef.h"
#include "regex.hpp"
#include "dfa.h"
#include "predef.h"
#include <string.h>
#include "yufashu.hpp"
#include <functional>
using std::string;
using std::cout;
using std::endl;
template <class char_type>
dfa<char_type> *get_dfa(std::istream &file)
{
    std::string str;
    std::string stri;
    nfa<char_type> as;
    while(1)
    {
        getline(file,str);
        auto b=str.begin();
        regex_node_block<nfa_state<char>> *q=read_regex_string<nfa_state<char>>(b,str.end());
        //q->print();
        getline(file,stri);
        if(stri=="-")
            q->write_last_name(std::string(str));
        else
            q->write_last_name(std::string(stri));

        q->write_to_map(as.start_state);

        //as.print();

        if(file.eof())
        {
            break;
        }
    }
     dfa<char_type> * pd=new dfa<char_type>(as);
     return pd;
}

int main()
{

    std::ifstream fileo("D:\\regex.txt",std::ios_base::in);
    std::ifstream filei("D:\\in.txt",std::ios_base::in);
    dfa<char> *a=get_dfa<char>(fileo);
    //freopen("D:\\asd.log","w",stdout);
    std::cout << "hang:" << a->state_map.size() << std::endl;/*
    a->read(std::istreambuf_iterator<char>(filei),std::istreambuf_iterator<char>(),
            [](auto a)
    {
        printf("%s:%s\n",a.type.c_str(),a.value.c_str());
    });*/
    std::ifstream c("D:\\pac.txt");
    grammar b(c);
    a->read(std::istreambuf_iterator<char>(filei),std::istreambuf_iterator<char>(),std::bind(&grammar::read,&b,std::placeholders::_1));/*
    b.read(gram_tree_node("id",""));
    b.read(gram_tree_node("(",""));
    b.read(gram_tree_node(")",""));*/
    b.read(gram_tree_node("",""));
    return 0;
}
