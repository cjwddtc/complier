#include "yufashu.hpp"
#include <istream>
#include <iostream>


project::project(input_type id_,const std::vector<input_type> &to_id_, input_type fin_id_,size_t pos_):
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
/*
void project::print()
{
    printf("%d:",from_id);
    size_t i=0;
    for(std::string_t n:*to_id)
    {
        if(i++==pos)
        {
            printf(".");
        }
        printf("%d",n);
    }
    printf("-%d\n",fin_id);
}*/


grammar::grammar(std::istream &file)
{
    std::string str;
    input_type a;
    std::vector<input_type> b;
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
                    b.push_back(str.substr(n));
                    break;
                }
                else
                {
                    b.push_back(str.substr(n,m-n));
                    n=++m;
                }
            }
            gram_map.insert(std::make_pair(a,b));
            b.clear();
        }
        else
        {
            a=str;
        }
    }/*
    std::vector<std::string> vec;
    vec.push_back(1);
    auto q=gram_map.insert(make_pair(0,vec));
    make_map(project("document",&(q->second),0));
    stack_state.push(0);*/
}

void grammar::get_first(std::string name,std::set<std::string> &set)
{
    if(gram_map.find(name)==gram_map.end())
    {
        set.insert(name);
    }
    else
    {
        auto itp=gram_map.equal_range(name);
        bool flag=true;
        for(auto it=itp.first; it!=itp.second; it++)
        {
			if (it->second.front() == name) {
				break;
			}
			else {
				get_first(it->second.front(), set);
			}
        }
    }
}

void grammar::expand(state_type type,state_set &ptr)
{
    if(type.pos!=type.to_id.size())
    {
        input_type other=type.to_id[type.pos];
        auto itp=gram_map.equal_range(other);
        std::set<input_type> set;
        if(type.pos+1==type.to_id.size())
        {
            set.insert(type.fin_id);
		}
		else {
			get_first(type.to_id[type.pos + 1],set);
		}
        for(auto it=itp.first; it!=itp.second; it++)
        {
            for(const std::string &id:set)
            {
                project prd(it->first,it->second,id);
                if(ptr.find(prd)==ptr.end())
                {
                    ptr.insert(prd);
                    expand(prd,ptr);
                }
            }
        }
    }
}

void grammar::next(state_set &ptr,next_map &map_)
{
    for(const state_type &a:ptr)
    {
        if(a.to_id.size()==a.pos)
        {
			specification s;
			s.name = a.from_id;
			s.size = a.to_id.size();
			map[this->get_id(ptr)][a.fin_id] = s;
        }
        else
        {
            auto b=a.to_id[a.pos];
            map_[b].insert(a.next());
        }
    }
}
/*
void grammar::read(gram_tree_node c)
{
    printf("reading:%s|\n",c.type.c_str());
    size_t a=id_m.get_id(c.type);
    if(stack_id.size()==0 && c.type=="")
    {
        stack_id.push(c);
        return ;
    }
    op b=map[stack_state.top()][a];
    if(b.type==1)
    {
        printf("in:%s\n",c.type.c_str());
        stack_state.push(b.size);
        stack_id.push(c);
    }
    else if(b.type==0)
    {
        gram_tree_node d(id_m.get_name(b.id),"");
        for(size_t i=0; i<b.size; i++)
        {
            printf("out:%s\n",stack_id.top().type.c_str());
            d.son_list.push_front(stack_id.top());
            stack_id.pop();
            stack_state.pop();
        }
        printf("guiyue:%s,son_size:%d\n",d.type.c_str(),d.son_list.size());
        fflush(stdout);
        read(d);
        if(b.id)
            read(c);
    }
    else if(b.type==255)
    {
        std::cout << "wrong!\n";
        while(!stack_id.empty())
        {
            std::cout << stack_id.top().type << ":" << stack_state.top() << std::endl;
            stack_id.pop();
            stack_state.pop();
        }
        throw a;
    }
    else
    {
        std::cout << "wrong!\n";
        while(!stack_id.empty())
        {
            std::cout << stack_id.top().type << ":" << stack_state.top() << std::endl;
            stack_id.pop();
            stack_state.pop();
        }
        throw b.type;
    }
}
*/

void grammar::link(state_set &from_set,state_set &to_set,input_type input)
{
	shift s;
	s.state = this->get_id(to_set);
    map[this->get_id(from_set)][input]=s;
}

void grammar::add_state(size_t n,state_set &ptr)
{
    if(map.size()!=n)
    {
        throw n;
    }
    else
    {
        map.resize(n+1);
    }
}
