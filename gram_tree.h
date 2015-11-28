#ifndef GRAM_TREE_H_INCLUDED
#define GRAM_TREE_H_INCLUDED
#include <string>
#include <list>
#include <iterator>
#include <vector>
#include "id_manager.h"

enum op_type {add_,or_,and_, minus_,assign_,call_,arg_,ret_,multiply_,divide_,compare_,greater_,greater_equal_,not_,goto_,if_goto_,push_,pop_,push_pos_,pop_pos_};


class code_line
{
public:
    typedef std::insert_iterator<std::list<code_line>> code_pos;
    enum op_type op;
    struct VALUE
    {
        size_t var[3];
        code_line *pos;
        VALUE();
    } value;
    size_t index;
    code_line(op_type op_);
    //void print();
};

class lsy_it:public std::insert_iterator<std::list<code_line>>
{
public:
    code_line *get_ptr();
};

class gram_tree_node
{
public:
    std::string type;
    std::string value;
    std::list<gram_tree_node> son_list;
    gram_tree_node(std::string type_,std::string value_);
    gram_tree_node(const gram_tree_node&a);
    void print(size_t n=0);
    typedef std::insert_iterator<std::list<code_line>> code_pos;
    code_pos &write_to_list(code_pos &it);
    typedef code_pos &(*fun_ptr)(code_pos &,gram_tree_node *);
    static std::vector<fun_ptr> fun_array;
    static id_manager *id_m;
    static fun_ptr *add_fun(std::string str);
};

#endif // GRAM_TREE_H_INCLUDED
