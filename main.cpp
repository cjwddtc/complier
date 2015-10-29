#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <fstream>
#include <assert.h>
#include "dfa.h"
#include "predef.h"
#include "cffx.hpp"
#include <string.h>
#include "regex_map_two.hpp"
using std::string;
using std::cout;
using std::endl;
template <class state_type,class char_type>
dfa<char_type,state_type> *get_dfa(std::istream &file){
    char stri[1024];
    char *str=stri;
    nfa<char_type> as;
    while(1){
        file.getline(str,1024);
        regex_node_block<nfa_state<char>> *q=read_regex_string<nfa_state<char>>(str,str+strlen(str));
        //q->print();
        file.getline(str,1024);
        q->write_last_name(std::string(str));
        q->write_to_map(*as.start_state.begin());
        dfa<char_type,state_type> qwe(as);
        if(file.eof()){
            break;
        }
    }
    return new dfa<char_type,state_type>(as);
}

int main()
{/*
    nfa<char> as;
    string str="_|(a-z)|(A-Z)(_|(a-z)|(A-Z)|(0-9))*";
    auto sta=str.begin();
    state_map<int,char> a;
    int n=0;
    regex_node_block<nfa_state<char>> *q=read_regex_string<nfa_state<char>>(sta,str.end());
    //q->print();
    //getchar();
    q->write_last_name("lsy");
    //q->print();
    q->write_to_map(*as.start_state.begin());
    as.print();
    dfa<char,int> qwe(as);
    std::ofstream fileo("D:\\asd.data",std::ios_base::binary|std::ios_base::out);
    qwe.write_to_file(fileo);
    fileo.close();
    std::ifstream filei("D:\\asd.data",std::ios_base::binary|std::ios_base::in);
    dfa<char,int> qw(filei);
    qwe.read('a');
    qwe.read('a');
    qwe.read('a');
    qwe.read('s');
    qwe.read('a');
    qwe.read('i');
    qwe.read(' ');
    std::cout <<qwe.result <<std::endl;
    /*
    for(int j=0;j<6;j++){
    for(int i=0;i<256;i++){
        if(qw.state_map[j][i]!=0){
            printf("%d,%d,%d\n",j,qw.state_map[j][i],i);
        }
    }
    }
    printf("%s\n",qw.finish_map[4].c_str());*/
    std::ifstream fileo("regex.txt",std::ios_base::in);
    std::ifstream filei("in.txt",std::ios_base::in);
    dfa<char,int> *a=get_dfa<int,char>(fileo);
    std::cout << "hang:" << a->state_map.size() << std::endl;
    a->read('\n');
    a->read_file(filei);
    std::cout << a->result << std::endl;
	getchar();
    return 0;
}
