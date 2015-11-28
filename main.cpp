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
#include "gram_fun.h"
using std::string;
using std::cout;
using std::endl;

int main()
{

    std::ifstream fileo("regex.txt",std::ios_base::in);
    std::ifstream filei("in.txt",std::ios_base::in);
    dfa<char> *a=get_dfa<char>(fileo);
    //freopen("D:\\asd.log","w",stdout);
    std::cout << "hang:" << a->state_map.size() << std::endl;/*
    a->read(std::istreambuf_iterator<char>(filei),std::istreambuf_iterator<char>(),
            [](auto a)
    {
        printf("%s:%s\n",a.type.c_str(),a.value.c_str());
    });*/
    std::ifstream c("pac.txt");
    grammar b(c);
    a->read(std::istreambuf_iterator<char>(filei),std::istreambuf_iterator<char>(),bind(&grammar::read,&b,std::placeholders::_1));
    b.read(gram_tree_node("",""));
    std::list<code_line> li;
    std::insert_iterator<std::list<code_line>> it(li,li.begin());
    gram_tree_node::id_m=&b.id_m;
    gram_tree_node::fun_array.resize(b.id_m.start_id+1);
    reg_fun();
    assert(*gram_tree_node::add_fun("all")!=0);
    b.stack_id.top().son_list.front().write_to_list(it);
    size_t i=0;
    for(code_line &a:li)
    {
        a.index=i++;
        printf("%d\n",a.op);
    }
    for(code_line &a:li)
    {
        switch (a.op)
        {
        case add_:
        case minus_:
        case multiply_:
        case divide_:
        case or_:
        case and_:
        case greater_:
        case greater_equal_:
        case compare_:
            std::cout << a.index << ":" << a.op << "|" << a.value.var[0] << "|" << a.value.var[1] << "|" << a.value.var[2] << endl;
            break;
        case not_:
        case assign_:
            std::cout << a.index << ":" << a.op << "|" << a.value.var[0] << "|" << a.value.var[1] << endl;
            break;
        case goto_:
            std::cout << a.index << ":goto" << a.value.pos->index+1 << endl;
            break;
        case if_goto_:
            std::cout << a.index << ":if"<< a.value.var[0] << "goto" << a.value.pos->index+1 << endl;
            break;
        }
    }
    return 0;
}
