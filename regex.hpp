#ifndef REGEX_HPP_INCLUDED
#define REGEX_HPP_INCLUDED
#include <list>

template <class write_map>
class regex_node_base
{
public:
    virtual ~regex_node_base()=default;
    typedef decltype((*(write_map*)nullptr)[0][0]) state_type;
    virtual state_type write_to_map(write_map &a,state_type b)=0;
};

template <class write_map>
class regex_node_char:public regex_node_base<write_map>
{
    typedef decltype((*(write_map*)nullptr)[0][0]) state_type;
public:
    char ch;
    regex_node_char(char ch_)
    {
        ch=ch_;
    }
    virtual state_type write_to_map(write_map &a,state_type b)
    {
        std::cout << typeid(*this).name() << std::endl;
    }
    virtual ~regex_node_char()=default;
};

template <class write_map>
class regex_node_block:public regex_node_base<write_map>
{
    typedef decltype((*(write_map*)nullptr)[0][0]) state_type;
public:
    std::list<regex_node_base<write_map>*> regex_nodes;
    void add(regex_node_base<write_map> *a)
    {
        regex_nodes.push_back(a);
    }
    virtual state_type write_to_map(write_map &a,state_type b)
    {
        std::cout << typeid(*this).name() << std::endl;
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
    typedef decltype((*(write_map*)nullptr)[0][0]) state_type;
public:
    regex_node_base<write_map> *repeat_source;
    unsigned int min;
    regex_node_repeat(regex_node_base<write_map> *a,unsigned int min_):
        repeat_source(a),min(min_) {}
    virtual state_type write_to_map(write_map &a,state_type b)
    {
        std::cout << typeid(*this).name() << std::endl;
    }
    virtual ~regex_node_repeat()
    {
        delete repeat_source;
    }
};

template <class write_map>
class regex_node_or:public regex_node_base<write_map>
{
    typedef decltype((*(write_map*)nullptr)[0][0]) state_type;
public:
    regex_node_base<write_map>* A;
    regex_node_base<write_map>* B;
    regex_node_or(regex_node_base<write_map> *a,regex_node_base<write_map> *b)
        :A(a),B(b) {}
    virtual state_type write_to_map(write_map &a,state_type b)
    {
        std::cout << typeid(*this).name() << std::endl;
    }
    virtual ~regex_node_or()
    {
        delete A;
        delete B;
    }

};

template <class write_map>
class regex_node_one_none:public regex_node_base<write_map>
{
    typedef decltype((*(write_map*)nullptr)[0][0]) state_type;
public:
    regex_node_base<write_map>* source;
    regex_node_one_none(regex_node_base<write_map>* source_):source(source_) {}
    virtual state_type write_to_map(write_map &a,state_type b)
    {
        std::cout << typeid(*this).name() << std::endl;
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
            regex_node_base<write_type> *&b=a->regex_nodes.back();
            b=new regex_node_repeat<write_type>(b,0);
            ++start;
        }
        break;
        case '+':
        {
            regex_node_base<write_type> *&b=a->regex_nodes.back();
            b=new regex_node_repeat<write_type>(b,1);
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
        break;
    case ')':
    case '*':
    case '+':
    case '|':
        throw *start;
        break;
    default:
        printf("read:%c\n",*start);
        re=new regex_node_char<write_type>(*start);
    }
    start++;
    return re;
}

#endif // REGEX_HPP_INCLUDED
