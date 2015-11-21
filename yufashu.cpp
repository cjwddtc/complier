#define DEBUG_MAP
#include "yufashu.hpp"
#include <istream>
#include <iostream>


project::project(size_t id_,const std::vector<size_t> *to_id_,size_t fin_id_,size_t pos_):
    from_id(id_),to_id(to_id_),fin_id(fin_id_),pos(pos_) {}


project project::next() const
{
    return project(from_id,to_id,fin_id,pos+1);
}


bool project::operator<(const project&a) const
{
    if(this->from_id!=a.from_id) return this->from_id<a.from_id;
    if(this->to_id!=a.to_id) return this->to_id<a.to_id;
    if(pos!=a.pos) return pos<a.pos;
    return fin_id<a.fin_id;
}
    void project::print(){
        printf("%d:",from_id);
        size_t i=0;
        for(size_t n:*to_id){
            if(i++==pos){
                printf(".");
            }
            printf("%d",n);
        }
        printf("-%d\n",fin_id);
    }

id_manager::id_manager():start_id(0){}
size_t id_manager::get_id(std::string str)
{
    if(name_map.find(str)==name_map.end())
    {
        name_map.insert(make_pair(str,start_id++));
        id_map.push_back(str);
    }
    return name_map[str];
}

std::string id_manager::get_name(size_t id){
    return id_map[id];
}

grammar::grammar(std::istream &file)
{
    std::string str;
    size_t a;
    std::vector<size_t> b;
    id_m.get_id("");
    while(!file.eof())
    {
        std::getline(file,str);
        if(str[0]=='\t')
        {
            size_t n=1;
            size_t m=1;
            while(true)
            {
                m=str.find_first_of(" \n",n);
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
    std::vector<size_t> vec;
    vec.push_back(1);
    auto q=gram_map.insert(make_pair(0,vec));
    make_map(project(0,&(q->second),0));
    stack_id.push(gram_tree_node("",""));
    stack_state.push(0);
}

bool grammar::get_first(size_t id,std::set<size_t> *set) const
{
    if(gram_map.find(id)==gram_map.end())
    {
        set->insert(id);
        return false;
    }
    else
    {
        auto itp=gram_map.equal_range(id);
        bool flag=true;
        for(auto it=itp.first; it!=itp.second; it++)
        {
            if(it->second.size()==0)
                flag=true;
            else if(it->second.front()==id)
                break;
            else
                if(get_first(it->second.front(),set)) flag=true;
        }
        return flag;
    }
}

void grammar::expand(state_type type,state_set *ptr)
{
    if(type.pos!=type.to_id->size())
    {
        input_type other=(*type.to_id)[type.pos];
        auto itp=gram_map.equal_range(other);
        //printf("search:%d\n",other);
        std::set<input_type> set;
        if(type.pos+1==type.to_id->size() || get_first((*type.to_id)[type.pos+1],&set))
        {
            set.insert(type.fin_id);
        }
        for(auto it=itp.first; it!=itp.second; it++)
        {
            for(size_t id:set)
            {
                /*printf("%d:\n",it->first);
                for(size_t n:(it->second)){
                    printf("%d ",n);
                }
                printf("\n");*/
                project prd(it->first,&(it->second),id);
                if(ptr->find(prd)==ptr->end())
                {
//                    printf("insert:\n");
//                    prd.print();
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
            op &b=map[this->get_id(ptr)][a.fin_id];
            b.type=0;
            b.size=a.to_id->size();
            b.id=a.from_id;
        }
        else {
            size_t b=(*a.to_id)[a.pos];
            map_[b].insert(a.next());
        }
    }
}

void grammar::read(gram_tree_node c){
    printf("reading:%s|\n",c.type.c_str());
    size_t a=id_m.get_id(c.type);
    if(stack_id.top().type=="all") stack_id.top().print(0);
    if(a==0 && stack_id.top().type==""){
        stack_id.top().print(0);
        return ;
    }
    op b=map[stack_state.top()][a];
    if(b.type==1){
        printf("in:%s\n",c.type.c_str());
        stack_state.push(b.size);
        stack_id.push(c);
    }else if(b.type==0){
        gram_tree_node d(id_m.get_name(b.id),"");
        for(size_t i=0;i<b.size;i++){
            printf("out:%s\n",stack_id.top().type.c_str());
            d.son_list.push_front(new gram_tree_node(stack_id.top()));
            stack_id.pop();
            stack_state.pop();
        }
        printf("guiyue:%s\n",d.type.c_str());
        fflush(stdout);
        read(d);
        if(b.id)
            read(c);
    }else if(b.type==255){
        std::cout << "wrong!\n";
        while(!stack_id.empty()){
            std::cout << stack_id.top().type << ":" << stack_state.top() << std::endl;
            stack_id.pop();
            stack_state.pop();
        }
        throw a;
    }else{
        std::cout << "wrong!\n";
        while(!stack_id.empty()){
            std::cout << stack_id.top().type << ":" << stack_state.top() << std::endl;
            stack_id.pop();
            stack_state.pop();
        }
        throw b.type;
    }
}


void grammar::link(state_set *from_set,state_set *to_set,input_type input)
{
    op &b=map[this->get_id(from_set)][input];
    b.type=1;
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

op::op():type(-1){}
