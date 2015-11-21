#include "gram_tree.h"
#include <iostream>

char white[1024]= {' '};

void gram_tree_node::print(size_t n)
{
    std::cout.write(white,n);

    std::cout <<"type:"<< type;
    if(value!="")
        std::cout <<":"<<"value:" << value << std::endl;
    else std::cout << std::endl;
    for(gram_tree_node *a:son_list)
    {
        a->print(n+1);
    }
}

gram_tree_node::gram_tree_node(std::string type_,std::string value_):type(type_),value(value_) {}

void gram_tree_node::free_son()
{
    for(gram_tree_node *a:son_list)
    {
        a->free_son();
        delete a;
    }
}

gram_tree_node::gram_tree_node(const gram_tree_node&a):type(a.type),value(a.value),son_list(a.son_list){
    //printf("copying\n");
}
