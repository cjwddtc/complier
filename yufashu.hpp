#include <map>
#include <vector>
#include <memory>
#include "array.h"
#include <set>

template <class id_type>
class production
{
public:
    id_type from_id;
    std::vector<id_type> *to_id;
    production(id_type id_,std::vector<id_type> *to_id_):from_id(id_),to_id(to_id_) {}
};

template <class id_type>
class project:public production<id_type>
{
public:
    id_type fin_id;
    size_t pos;
    project(id_type id_,std::vector<id_type> *to_id_,id_type fin_id_,size_t pos_=0):production<id_type>(id_,to_id_),fin_id(fin_id_),pos(pos_) {}
    project<id_type> &&next(){
        return project<id_type>(production<id_type>::froem_id,production<id_type>::to_id,fin_id,pos+1);
    }
};

template <class id_type>
using projects=std::set<project<id_type>>;

template <class id_type>
class grammar
{
public:
    std::multimap<id_type,std::vector<id_type>> map;
    id_type max_id;
    void get_first(id_type id,std::set<id_type> *set)
    {
        if(map.find(id)==map.end())
        {
            set->insert(id);
        }
        else
        {
            auto itp=map.equal_range(id);
            for(auto it=itp.first; it!=itp.second; it++)
            {
                if(it->first==id){
                    return;
                }
                if(it->second.size()==0)
                    set->insert(get_null_value<id_type>());
                else
                    get_first(it->second.front(),set);
            }
        }
    }
    //递归将a项目相关的项目加入source中
    void add_to(project<id_type> &a,projects<id_type>* source)
    {
        if(a.pos!=a.pro.to_id.size())
        {
            id_type other=a.pro[a.pos];
            auto itp=map.equal_range(other);
            std::set<id_type> set;
            if(a.pos+1==a.pro.to_id.size())
            {
                set.insert(get_null_value<id_type>());
            }
            else
            {
                get_first(a.pro[a.pos+1],&set);
            }
            bool have_null=(set.find(get_null_value<id_type>())==set.end());
            set.erase(get_null_value<id_type>());
            for(auto it=itp->first; it!=itp->second; it++)
            {
                if(have_null)
                {
                    set.insert(a.fin_id);
                }
                for(id_type id:set)
                {
                    project<id_type> prd(it->first,it->second,id);
                    if(source->find(prd)==source->end())
                    {
                        source->insert(prd);
                        add_to(prd,source);
                    }
                }
            }
        }
    }
    //闭包
    projects<id_type> extend(projects<id_type>* source)
    {
        projects<id_type> ptr=new projects<id_type>(*source);
        for(project<id_type> &b:source)
        {
            add_to(b,ptr.get());
        }
        delete source;
        return ptr;
    }
};



template <class id_type>
class op
{
    size_t flag;
    id_type id;
};

template <class T>
struct ptr_less{
    bool operator()(const T *a,const T *b){
        return *a<*b;
    }
};

template <class id_type>
class parser
{
    std::vector<array<op<id_type>>> map;
    parser(const grammar<id_type> &gramer)
    {
        std::map<projects<id_type> *,size_t,ptr_less<projects<id_type>>> all_id_map;
        std::map<projects<id_type> *,size_t,ptr_less<projects<id_type>>> aread_id_map;
        std::set<project<id_type>> *projec=new std::set<project<id_type>>();
        auto b=gramer.map.find(get_null_value<id_type>());
        if(b==gramer.map.end()){
            throw gramer.map;
        }
        projec->insert(project<id_type>(get_null_value<id_type>(),&*b,get_null_value<id_type>()));
        projects<id_type> *ptr=gramer.extend(projec);
        //all_id_map.insert()
        bool flag=true;
        while(flag){
            std::map<id_type,projects<id_type> *>
            flag=false;
            for(project<id_type> &pro:*ptr){
                if(p)
            }
        }
    }
};
