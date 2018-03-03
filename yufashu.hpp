#pragma once
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <stack>
#include "template.hpp"
#include <string>
#include <variant>
typedef std::string input_type;

class project
{
public:
	input_type from_id;
    const std::vector<input_type> &to_id;
	input_type fin_id;
    size_t pos;
    project(input_type id_,const std::vector<input_type> &to_id_,input_type fin_id_,size_t pos_=0);
    project next() const;
    bool operator<(const project &a) const;
    //void print();
};

//ÒÆ½ø
struct shift
{
	size_t state;
};
//¹æÔ¼
struct specification
{
	size_t size;
	std::string name;
};
typedef std::variant<shift, specification> op;
typedef project state_type;

class grammar:public state_to_map<state_type, input_type>
{
public:
    typedef typename state_to_map<state_type,input_type>::state_set state_set;
    typedef typename state_to_map<state_type, input_type>::next_map next_map;
    std::multimap<input_type,std::vector<input_type>> gram_map;
    std::vector<std::map<std::string,op>> map;
    std::stack<std::string> stack_state;
    //std::stack<gram_tree_node> stack_id;
	grammar(std::istream &file);
    void get_first(std::string id,std::set<std::string> &set);
    virtual void expand(state_type type,state_set &ptr);
    virtual void next(state_set &ptr,next_map &map_);
    virtual void link(state_set &from_set,state_set &to_set,input_type input);
    virtual void add_state(size_t n,state_set &ptr);
    //void read(gram_tree_node b);
};
