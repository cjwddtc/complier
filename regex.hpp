#ifndef REGEX_HPP_INCLUDED
#define REGEX_HPP_INCLUDED
#include <list>

template <class write_map>
class regex_node_base
{
public:
    virtual ~regex_node_base()=default;
    typedef typename write_map::type_state state_type;
    //this func is to convert regex to map by recurse
    //the b is input state return the state after the func called
    virtual state_type &write_to_map(write_map &a,state_type &b,state_type fin_state=0)=0;
};

template <class write_map>
class regex_node_char:public regex_node_base<write_map>
{
    typedef typename regex_node_base<write_map>::state_type state_type;
    typedef typename write_map::value_type line;
public:
    char ch;
    regex_node_char(char ch_)
    {
        ch=ch_;
    }
    virtual state_type &write_to_map(write_map &a,state_type &b,,state_type fin_state=0)
    {
        if(fin_state)
        {
            a[b][ch]=fin_state;
        }
        else
        {
            state_type &c=a[b][ch];
            if(c==0)
            {
                c=a.add_A(line());
            }
        }
        return a[b][ch];
    }
    virtual ~regex_node_char()=default;
};

template <class write_map>
class regex_node_block:public regex_node_base<write_map>
{
    typedef typename regex_node_base<write_map>::state_type state_type;
public:
    std::list<regex_node_base<write_map>*> regex_nodes;
    void add(regex_node_base<write_map> *a)
    {
        regex_nodes.push_back(a);
    }
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        state_type *ptr=&b;
        for(regex_node_base<write_map>* c:regex_nodes)
        {
            ptr=&c->write_to_map(a,*ptr);
        }
        return *ptr;
    }
    virtual ~regex_node_block()
    {
        for(regex_node_base<write_map> *a:regex_nodes)
        {
            delete a;
        }
    }
};

template <class write_map>
class regex_node_repeat:public regex_node_base<write_map>
{
    typedef typename regex_node_base<write_map>::state_type state_type;
public:
    regex_node_base<write_map> *repeat_source;
    regex_node_repeat(regex_node_base<write_map> *a):
        repeat_source(a) {}
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        state_type &c=repeat_source->write_to_map(a,b);

    }
    virtual ~regex_node_repeat()
    {
        delete repeat_source;
    }
};

template <class write_map>
class regex_node_or:public regex_node_base<write_map>
{
    typedef typename regex_node_base<write_map>::state_type state_type;
public:
    regex_node_base<write_map>* A;
    regex_node_base<write_map>* B;
    regex_node_or(regex_node_base<write_map> *a,regex_node_base<write_map> *b)
        :A(a),B(b) {}
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        std::cout << "regex_node_or" << std::endl;
    }
    virtual ~regex_node_or()
    {
        delete A;
        delete B;
    }

};

template <class write_map>
class regex_node_range:public regex_node_base<write_map>
{
    typedef typename regex_node_base<write_map>::state_type state_type;
    regex_node_char<write_map> *left;
    regex_node_char<write_map> *right;
public:
    regex_node_range(regex_node_char<write_map> *a,regex_node_char<write_map> *b):left(a),right(b) {}
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        std::cout << "regex_node_range" << std::endl;
    }
    ~regex_node_range()
    {
        delete right;
        delete left;
    }
};

template <class write_map>
class regex_node_one_none:public regex_node_base<write_map>
{
    typedef typename regex_node_base<write_map>::state_type state_type;
public:
    regex_node_base<write_map>* source;
    regex_node_one_none(regex_node_base<write_map>* source_):source(source_) {}
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        std::cout << "regex_node_one_none" << std::endl;
    }
    ~regex_node_one_none()
    {
        delete source;
    }

};

template <class write_type,class ITERATOR>
regex_node_base<write_type> *read_regex_unit(ITERATOR &start,ITERATOR finish);

template <class write_type,class ITERATOR>
regex_node_block<write_type> *read_regex_string(ITERATOR &start,ITERATOR finish)
{
    regex_node_block<write_type> *a=new regex_node_block<write_type>();
    while(start!=finish)
    {
        switch(*start)
        {
        case ')':
            ++start;
            return a;
        case '*':
        {
            regex_node_base<write_type> *b=a->regex_nodes.back();
            a->add(new regex_node_repeat<write_type>(b));
            ++start;
        }
        break;
        case '+':
        {
            regex_node_base<write_type> *&b=a->regex_nodes.back();
            b=new regex_node_repeat<write_type>(b);
            ++start;
        }
        break;
        case '?':
        {
            regex_node_base<write_type> *&b=a->regex_nodes.back();
            b=new regex_node_one_none<write_type>(b);
            ++start;
        }
        break;
        case '|':
        {
            regex_node_base<write_type> *&b=a->regex_nodes.back();
            b=new regex_node_or<write_type>(b,read_regex_unit<write_type>(++start,finish));
        }
        break;
        case '-':
        {
            regex_node_base<write_type> *&b=a->regex_nodes.back();
            regex_node_char<write_type> *c=dynamic_cast<regex_node_char<write_type> *>(b);
            if(c==0)
            {
                throw b;
            }
            regex_node_base<write_type> *e=read_regex_unit<write_type>(++start,finish);
            regex_node_char<write_type> *d=dynamic_cast<regex_node_char<write_type> *>(b);
            if(d==0)
            {
                throw e;
            }
            b=new regex_node_range<write_type>(c,d);
        }
        break;
        default:
            a->add(read_regex_unit<write_type>(start,finish));
        }
    }
    return a;
}

template <class write_type,class ITERATOR>
regex_node_base<write_type> *read_regex_unit(ITERATOR &start,ITERATOR finish)
{
    regex_node_base<write_type> *re;
    switch(*start)
    {
    case '\\':
        re=new regex_node_char<write_type>(*++start);
        break;
    case '(':
        re=read_regex_string<write_type>(++start,finish);
        --start;
        break;
    case ')':
    case '*':
    case '+':
    case '|':
        throw *start;
        break;
    default:
        re=new regex_node_char<write_type>(*start);
    }
    start++;
    return re;
}

#endif // REGEX_HPP_INCLUDED
