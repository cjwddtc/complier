#ifndef GRAM_TREE_H_INCLUDED
#define GRAM_TREE_H_INCLUDED
#include <string>
#include <list>
#include <iterator>

class gram_tree_node{
public:
    std::string type;
    std::string value;
    std::list<gram_tree_node *>son_list;
    gram_tree_node(std::string type_,std::string value_);
    gram_tree_node(const gram_tree_node&a);
    void print(size_t n=0);
    void free_son();
    typedef std::insert_iterator<std::list<std::string>> code_pos;
    code_pos write_to_list(code_pos it);
};

#endif // GRAM_TREE_H_INCLUDED
