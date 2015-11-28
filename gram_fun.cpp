#include "gram_tree.h"
#include <functional>
#include <assert.h>
#include <iostream>


id_manager symbol_map;
std::map<std::string,code_line *> fun_map;
//std::map<std::string,gram_tree_node::code_pos>

void reg_fun()
{
    *gram_tree_node::add_fun("all")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        if(ptr->son_list.size()==1)
        {
            return ptr->son_list.front().write_to_list(pos);
        }
        else if(ptr->son_list.size()==2)
        {
            return ptr->son_list.back().write_to_list(ptr->son_list.front().write_to_list(pos));
        }
        else{
            throw ptr;
        }
    };
    *gram_tree_node::add_fun("fun_imp")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        ++a;
        gram_tree_node *b=&*a;
        while(b->son_list.size()==2)
        {
            b=&b->son_list.back();
        }
        b=&b->son_list.back();
        {
            auto a=b->son_list.begin();
            fun_map[a->value]=((lsy_it *)&pos)->get_ptr();
            if(++a==b->son_list.end()) throw ptr;
            if(a->type!="(") throw ptr;
            ++a;
            if(a->type=="arg_list")
            {
                pos=a->write_to_list(pos);
            }
        }
        ++a;
        ++a;
        code_line c(pop_pos_);
        pos=c;
        pos=a->write_to_list(pos);
        return pos;
    };
    *gram_tree_node::add_fun("arg_list")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type=="arg_list")
        {
            pos=a->write_to_list(pos);
            ++a;
            ++a;
        }
        ++a;
        gram_tree_node &c=a->son_list.front().son_list.back();
        if(c.son_list.front().type=="id")
        {
            code_line b(pop_);
            b.value.var[0]=symbol_map.get_id(c.son_list.front().value);
            pos=b;
            return pos;
        }
        throw ptr;
    };
    *gram_tree_node::add_fun("expressions")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type=="expressions")
        {
            pos=a->write_to_list(pos);
            ++a;
        }
        return a->write_to_list(pos);
    };
    *gram_tree_node::add_fun("expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type==";")return pos;
        if(a->type=="{")
        {
            ++a;
            return a->write_to_list(pos);
        }
        if(a->type=="return")
        {
            ++a;
            pos=a->write_to_list(pos);
            code_line c(push_);
            c.value.var[0]=symbol_map.get_front_id();
            pos=c;
            return pos;
        }
        return a->write_to_list(pos);
    };
    *gram_tree_node::add_fun("dot_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        for(gram_tree_node &node:ptr->son_list)
        {
            if(node.type!=",")
            {
                pos=node.write_to_list(pos);
            }
        }
        return pos;
    };
    *gram_tree_node::add_fun("assign_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type=="compare_expression")return a->write_to_list(pos);
        pos=a->write_to_list(pos);
        size_t t1=symbol_map.get_front_id();
        ++a;
        ++a;
        pos=a->write_to_list(pos);
        size_t t2=symbol_map.get_front_id();
        code_line c(assign_);
        c.value.var[0]=symbol_map.get_new_id();
        c.value.var[1]=t1;
        c.value.var[2]=t2;
        pos=c;
        return pos;
    };
    *gram_tree_node::add_fun("compare_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type=="logic_expression")return a->write_to_list(pos);
        pos=a->write_to_list(pos);
        size_t t1=symbol_map.get_front_id();
        ++a;
        enum op_type va;
        bool is_change=false;
        if(a->value=="==") va=compare_;
        else if(a->value==">=") va=greater_equal_;
        else if(a->value==">") va=greater_;
        else {
            if(a->value=="<=") va=greater_equal_;
            else if(a->value=="<") va=greater_;
            else assert(0);
            is_change=true;
        }
        ++a;
        pos=a->write_to_list(pos);
        size_t t2=symbol_map.get_front_id();
        code_line c(compare_);
        c.value.var[0]=symbol_map.get_new_id();
        if(is_change)
        {
            c.value.var[1]=t2;
            c.value.var[2]=t1;
        }
        else{
            c.value.var[1]=t1;
            c.value.var[2]=t2;
        }
        pos=c;
        return pos;
    };
    *gram_tree_node::add_fun("logic_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type=="move_expression")return a->write_to_list(pos);
        pos=a->write_to_list(pos);
        size_t t1=symbol_map.get_front_id();
        ++a;
        enum op_type va;
        if(a->type=="logic")
        {
            if(a->value=="&&") va=and_;
            else if(a->value=="||") va=or_;
            throw *a;
        }
        else assert(0);
        ++a;
        pos=a->write_to_list(pos);
        size_t t2=symbol_map.get_front_id();
        code_line c(va);
        c.value.var[0]=symbol_map.get_new_id();
        c.value.var[1]=t1;
        c.value.var[2]=t2;
        pos=c;
        return pos;
    };
    *gram_tree_node::add_fun("move_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        assert(ptr->son_list.size()==1);
        return ptr->son_list.back().write_to_list(pos);
    };
    *gram_tree_node::add_fun("additive_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type=="multiplicative_expression")return a->write_to_list(pos);
        pos=a->write_to_list(pos);
        size_t t1=symbol_map.get_front_id();
        ++a;
        enum op_type va;
        if(a->value=="+") va=add_;
        else if(a->value=="-") va=minus_;
        throw a->value;
        ++a;
        pos=a->write_to_list(pos);
        size_t t2=symbol_map.get_front_id();
        code_line c(va);
        c.value.var[0]=symbol_map.get_new_id();
        c.value.var[1]=t1;
        c.value.var[2]=t2;
        pos=c;
        return pos;
    };
    *gram_tree_node::add_fun("multiplicative_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        if(a->type=="cast_expression")return a->write_to_list(pos);
        pos=a->write_to_list(pos);
        size_t t1=symbol_map.get_front_id();
        ++a;
        enum op_type va;
        if(a->value=="*") va=multiply_;
        else if(a->value=="/") va=divide_;
        throw a->value;
        ++a;
        pos=a->write_to_list(pos);
        size_t t2=symbol_map.get_front_id();
        code_line c(va);
        c.value.var[0]=symbol_map.get_new_id();
        c.value.var[1]=t1;
        c.value.var[2]=t2;
        pos=c;
        return pos;
    };
    *gram_tree_node::add_fun("cast_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        assert(ptr->son_list.size()==1);
        return ptr->son_list.back().write_to_list(pos);
    };
    *gram_tree_node::add_fun("unary_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        assert(ptr->son_list.size()==1);
        return ptr->son_list.back().write_to_list(pos);
    };
    *gram_tree_node::add_fun("postfix_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        if(ptr->son_list.size()!=1)
        {
            std::cout << ptr->son_list.back().type << std::endl;
            assert(0);
        }
        return ptr->son_list.back().write_to_list(pos);
    };
    *gram_tree_node::add_fun("primary_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        if(ptr->son_list.front().type!="id")
        {
            std::cout << ptr->son_list.front().type << std::endl;
            assert(0);
        }
        symbol_map.get_id(ptr->son_list.front().value);
        return pos;
    };
    *gram_tree_node::add_fun("declaration")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        return pos;
    };
    *gram_tree_node::add_fun("if_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        ++a;
        ++a;
        pos=a->write_to_list(pos);
        size_t t1=symbol_map.get_front_id();
        code_line c(not_);
        c.value.var[0]=symbol_map.get_new_id();
        c.value.var[1]=t1;
        pos=c;
        code_line d(if_goto_);
        d.value.var[0]=symbol_map.get_front_id();
        pos=d;
        code_line *p=((lsy_it*)&pos)->get_ptr();
        ++a;
        ++a;
        pos=a->write_to_list(pos);
        code_line e(goto_);
        e.value.pos=0;
        pos=e;
        code_line *pi=((lsy_it*)&pos)->get_ptr();
        p->value.pos=((lsy_it*)&pos)->get_ptr();
        ++a;
        if(a!=ptr->son_list.end())
        {
            ++a;
            pos=c;
            pos=a->write_to_list(pos);
        }
        pi->value.pos=((lsy_it*)&pos)->get_ptr();
        return pos;
    };
    *gram_tree_node::add_fun("while_expression")=[](gram_tree_node::code_pos &pos,gram_tree_node *ptr)->gram_tree_node::code_pos &
    {
        auto a=ptr->son_list.begin();
        ++a;
        ++a;
        pos=a->write_to_list(pos);
        size_t t1=symbol_map.get_front_id();
        code_line c(not_);
        c.value.var[0]=symbol_map.get_new_id();
        c.value.var[0]=t1;
        pos=c;
        code_line *pi=((lsy_it*)&pos)->get_ptr();
        code_line d(if_goto_);
        d.value.var[0]=symbol_map.get_front_id();
        pos=d;
        code_line *p=((lsy_it*)&pos)->get_ptr();
        ++a;
        ++a;
        pos=a->write_to_list(pos);
        code_line e(goto_);
        e.value.pos=pi;
        pos=e;
        p->value.pos=((lsy_it*)&pos)->get_ptr();
        return pos;
    };
}
