#include <map>
#include <vector>
#include <memory>
#include "array.h"
#include <string>
#include <set>
#include <stack>
#include "template.hpp"
#include "gram_tree.h"

class project
{
public:
    size_t from_id;
    const std::vector<size_t> *to_id;
    size_t fin_id;
    size_t pos;
    project(size_t id_,const std::vector<size_t> *to_id_,size_t fin_id_,size_t pos_=0);
    project next() const;
    bool operator<(const project &a) const;
    void print();
};


class op
{
public:
    size_t size;
    size_t id;
    unsigned char type;
    op();
};


class grammar:public state_to_map<project,size_t>
{
protected:
    using state_to_map<project,size_t>::state_to_map;
    using state_to_map<project,size_t>::get_id;
public:
    typedef project state_type;
    typedef size_t input_type;
    typedef typename state_to_map<project,size_t>::state_set state_set;
    typedef typename state_to_map<project,size_t>::next_map next_map;
    std::multimap<size_t,std::vector<size_t>> gram_map;
    std::vector<std::vector<op>> map;
    id_manager id_m;
    std::stack<size_t> stack_state;
    std::stack<gram_tree_node> stack_id;
    grammar(std::istream &file);
    bool get_first(size_t id,std::set<size_t> *set) const;
    virtual void expand(state_type type,state_set_ptr ptr);
    virtual void next(state_set_ptr ptr,next_map &map_);
    virtual void link(state_set_ptr from_set,state_set_ptr to_set,input_type input);
    virtual void add_state(size_t n,state_set_ptr ptr);
    //void read(size_t a);
    void read(gram_tree_node b);
};
