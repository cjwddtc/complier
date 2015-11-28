#include "gram_tree.h"
#include <iostream>
#include <algorithm>
#include <assert.h>

char white[1024]= {' '};


code_line::code_line(op_type op_):op(op_),value() {}
code_line::VALUE::VALUE() {}

code_line *lsy_it::get_ptr()
{
    auto b=iter;
    return &*--b;
}

void gram_tree_node::print(size_t n)
{
    std::cout.write(white,n);

    std::cout <<"type:"<< type;
    if(value!="")
        std::cout <<":"<<"value:" << value << std::endl;
    else std::cout << std::endl;
    if(son_list.size()!=0)
    {
        std::cout.write(white,n);
        std::cout << "sons:" << son_list.size() << std::endl;
        for(gram_tree_node &a:son_list)
        {
            a.print(n+1);
        }
    }
}

gram_tree_node::gram_tree_node(std::string type_,std::string value_):type(type_),value(value_) {}

gram_tree_node::gram_tree_node(const gram_tree_node&a):type(a.type),value(a.value),son_list(a.son_list)
{
    //printf("copying\n");
}

gram_tree_node::code_pos &gram_tree_node::write_to_list(gram_tree_node::code_pos &it)
{
    if(*add_fun(type)==0)
    {
        std::cout << type << ":not found" << std::endl;
        assert(0);
    }
    std::cout << type << ":create" << std::endl;
    return (*add_fun(type))(it,this);
}


gram_tree_node::fun_ptr *gram_tree_node::add_fun(std::string str)
{
    size_t n=id_m->get_id(str);
    return &fun_array[n];
}

id_manager *gram_tree_node::id_m=0;
std::vector<gram_tree_node::fun_ptr> gram_tree_node::fun_array;
