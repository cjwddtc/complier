#ifndef GRAM_TREE_H_INCLUDED
#define GRAM_TREE_H_INCLUDED
#include <string>
#include <list>

class gram_tree_node{
public:
    std::string type;
    std::string value;
    std::list<gram_tree_node *>son_list;
    gram_tree_node(std::string type_,std::string value_);
    gram_tree_node(const gram_tree_node&a);
    void print(size_t n);
    void free_son();
};

#endif // GRAM_TREE_H_INCLUDED
