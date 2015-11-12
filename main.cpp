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
using std::string;
using std::cout;
using std::endl;
template <class char_type>
dfa<char_type> *get_dfa(std::istream &file)
{
    char stri[1024];
    char *str=stri;
    nfa<char_type> as;
    while(1)
    {
        file.getline(str,1024);
        regex_node_block<nfa_state<char>> *q=read_regex_string<nfa_state<char>>(str,str+strlen(str));
        //q->print();
        file.getline(str,1024);
        q->write_last_name(std::string(str));
        q->write_to_map(as.start_state);
        dfa<char_type> qwe(as);
        if(file.eof())
        {
            break;
        }
    }
    return new dfa<char_type>(as);
}

int main()
{/*
    std::ifstream fileo("regex.txt",std::ios_base::in);
    std::ifstream filei("in.txt",std::ios_base::in);
    dfa<char> *a=get_dfa<char>(fileo);
    std::cout << "hang:" << a->state_map.size() << std::endl;
    a->read(std::istreambuf_iterator<char>(filei),std::istreambuf_iterator<char>(),
            [](std::pair<std::string,std::string> a)
    {
        printf("%s:%s\n",a.first.c_str(),a.second.c_str());
    });
    std::cout << a->result << std::endl;*/
    std::ifstream c("D:\\pac.txt");
    grammar a(c);

    return 0;
}
