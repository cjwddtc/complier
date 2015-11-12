#include <map>
#include <vector>
#include <memory>
#include "array.h"
#include <string>
#include <set>
#include "template.hpp"

class project
{
public:
    size_t from_id;
    const std::vector<size_t> *to_id;
    size_t fin_id;
    size_t pos;
    project(size_t id_,const std::vector<size_t> *to_id_,size_t fin_id_,size_t pos_=0);
    project &&next() const;
    bool operator<(const project &a) const;
};

class id_manager{
    std::map<std::string,size_t> name_map;
public:
    size_t start_id;
    size_t get_id(std::string str);
};

class op
{
public:
    bool type;
    size_t size;
    size_t id;
};

class grammar:public state_to_map<project,size_t>
{
public:
    typedef project state_type;
    typedef size_t input_type;
    typedef typename state_to_map<project,size_t>::state_set state_set;
    typedef typename state_to_map<project,size_t>::next_map next_map;
    std::multimap<size_t,std::vector<size_t>> gram_map;
    std::vector<array<op>> map;
    id_manager id_m;
    grammar(std::istream &file);
    void get_first(size_t id,std::set<size_t> *set) const;
    virtual void expand(state_type type,state_set *ptr);
    virtual void next(state_set *ptr,next_map &map_);
    virtual void link(state_set *from_set,state_set *to_set,input_type input);
    virtual void add_state(size_t n,state_set *ptr);
};
