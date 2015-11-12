#include "yufashu.hpp"
#include <istream>
project::project(size_t id_,const std::vector<size_t> *to_id_,size_t fin_id_,size_t pos_):
    from_id(id_),to_id(to_id_),fin_id(fin_id_),pos(pos_) {}


project &&project::next() const
{
    return project(from_id,to_id,fin_id,pos+1);
}


bool project::operator<(const project&a) const
{
    if(this->from_id!=a.from_id) return this->from_id<a.from_id;
    if(this->to_id!=a.to_id) return this->to_id<a.to_id;
    if(pos!=a.pos) return pos<a.pos;
    if(fin_id!=a.fin_id) return fin_id<a.fin_id;
}


id_manager::id_manager():start_id(1){}
size_t id_manager::get_id(std::string str)
{
    if(name_map.find(str)==name_map.end())
    {
        name_map.insert(make_pair(str,start_id++));
    }
    return name_map[str];
}

grammar::grammar(std::istream &file)
{
    std::string str;
    size_t a;
    std::vector<size_t> b;
    while(!std::getline(file,str).eof())
    {
        if(str[0]=='\t')
        {
            size_t n=0;
            size_t m=0;
            while(true)
            {
                m=str.find_first_of(' ',n);
                if(m==std::string::npos)
                {
                    b.push_back(id_m.get_id(str.substr(n)));
                    break;
                }
                else
                {
                    b.push_back(id_m.get_id(str.substr(n,m-n)));
                    n=++m;
                }
            }
            gram_map.insert(std::make_pair(a,b));
            b.clear();
        }
        else
        {
            a=id_m.get_id(str);
        }
    }
}

void grammar::get_first(size_t id,std::set<size_t> *set) const
{
    if(gram_map.find(id)==gram_map.end())
    {
        set->insert(id);
    }
    else
    {
        auto itp=gram_map.equal_range(id);
        for(auto it=itp.first; it!=itp.second; it++)
        {
            if(it->first==id)
            {
                return;
            }
            if(it->second.size()==0)
                set->insert(0);
            else
                get_first(it->second.front(),set);
        }
    }
}

void grammar::expand(state_type type,state_set *ptr)
{
    if(type.pos!=type.to_id->size())
    {
        input_type other=(*type.to_id)[type.pos];
        auto itp=gram_map.equal_range(other);
        std::set<input_type> set;
        if(type.pos+1==type.to_id->size())
        {
            set.insert(0);
        }
        else
        {
            get_first((*type.to_id)[type.pos+1],&set);
        }
        if(set.find(0)==set.end())
        {
            set.insert(type.fin_id);
        }
        set.erase(0);
        for(auto it=itp.first; it!=itp.second; it++)
        {
            for(size_t id:set)
            {
                project prd(it->first,&(it->second),id);
                if(ptr->find(prd)==ptr->end())
                {
                    ptr->insert(prd);
                    expand(prd,ptr);
                }
            }
        }
    }
}

void grammar::next(state_set *ptr,next_map &map_)
{
    for(const state_type &a:*ptr)
    {
        if(a.to_id->size()==a.pos)
        {
            auto b=map[this->get_id(ptr)][a.fin_id];
            b.type=false;
            b.size=a.to_id->size();
            b.id=a.from_id;
        }
        else map_[(*a.to_id)[a.pos]].insert(a.next());
    }
}

void grammar::link(state_set *from_set,state_set *to_set,input_type input)
{
    auto b=map[this->get_id(from_set)][input];
    b.type=true;
    b.size=this->get_id(to_set);
}

void grammar::add_state(size_t n,state_set *ptr)
{
    if(map.size()!=n)
    {
        throw n;
    }
    else
    {
        map.push_back(array<op>(id_m.start_id));
    }
}
