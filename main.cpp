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
    a->read(std::istreambuf_iterator<char>(filei),std::istreambuf_iterator<char>(),[&b](auto a){b.read(a);});
    b.read(gram_tree_node("",""));
    b.stack_id.top().print();
    return 0;
}
