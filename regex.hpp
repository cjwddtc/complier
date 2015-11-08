#ifndef REGEX_HPP_INCLUDED
#define REGEX_HPP_INCLUDED
#include <list>

template <class write_map>
class regex_node_base
{
public:
    virtual ~regex_node_base()=default;
    typedef typename write_map::type_char char_type;
    //this func is to convert regex to map by recurse
    //the b is input state return the state after the func called
    virtual write_map *write_to_map(write_map *a)=0;
    virtual void write_last_name(std::string name)=0;
    virtual void print(int tab=0)=0;
};

template <class write_map>
class regex_node_char:public regex_node_base<write_map>
{
    typedef typename write_map::type_char char_type;
public:
    char_type ch;
    std::string name;
    regex_node_char(char_type ch_)
    {
        ch=ch_;
    }
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *s=new write_map(name);
        a->link(ch,s);
        return s;
    }
    virtual void write_last_name(std::string name)
    {
        this->name=name;
    }
    virtual void print(int tab=0)
    {
        for(int i=0; i<tab; i++)
        {
            printf(" ");
        }
        printf("regex_node_char:%d\n",ch);
    }
    virtual ~regex_node_char()=default;
};
template <class write_map>
class regex_node_not_char:public regex_node_base<write_map>
{
    typedef typename write_map::type_char char_type;
public:
    char_type ch;
    std::string name;
    regex_node_not_char(char_type ch_)
    {
        ch=ch_;
    }
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *s=new write_map(name);
        for(char_type ch=get_range_low<char_type>(); ch<get_range_hight<char_type>(); ch++)
            if(ch!=this->ch)
                a->link(ch,s);
        return s;
    }
    virtual void write_last_name(std::string name)
    {
        this->name=name;
    }
    virtual void print(int tab=0)
    {
        for(int i=0; i<tab; i++)
        {
            printf(" ");
        }
        printf("regex_node_not_char:%d\n",ch);
    }
    virtual ~regex_node_not_char()=default;
};

template <class write_map>
class regex_node_block:public regex_node_base<write_map>
{
    typedef typename write_map::type_char char_type;
public:
    std::list<regex_node_base<write_map>*> regex_nodes;
    void add(regex_node_base<write_map> *a)
    {
        regex_nodes.push_back(a);
    }
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *ptr=a;
        for(regex_node_base<write_map>* b:regex_nodes)
        {
            ptr=b->write_to_map(ptr);
        }
        return ptr;
    }
    virtual void write_last_name(std::string name)
    {
        regex_nodes.back()->write_last_name(name);
    }
    virtual void print(int tab=0)
    {
        for(int i=0; i<tab; i++)
        {
            printf(" ");
        }
        printf("regex_node_block\n");
        tab++;
        for(regex_node_base<write_map>* a:regex_nodes)
        {
            a->print(tab);
        }
    }
    virtual ~regex_node_block()
    {
        for(regex_node_base<write_map> *a:regex_nodes)
        {
            delete a;
        }
    }
};
//重复任意次 至少一次
template <class write_map>
class regex_node_repeat:public regex_node_base<write_map>
{
    typedef typename write_map::type_char char_type;
public:
    regex_node_base<write_map> *repeat_source;
    regex_node_repeat(regex_node_base<write_map> *a):
        repeat_source(a) {}
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *b=new write_map();
        write_map *c=repeat_source->write_to_map(b);
        c->link(get_null_value<char_type>(),b);
        a->link(get_null_value<char_type>(),b);
        return c;
    }
    virtual void write_last_name(std::string name)
    {
        repeat_source->write_last_name(name);
    }
    virtual void print(int tab=0)
    {
        for(int i=0; i<tab; i++)
        {
            printf(" ");
        }
        printf("regex_node_repeat\n");
        repeat_source->print(++tab);
    }
    virtual ~regex_node_repeat()
    {
        delete repeat_source;
    }
};

template <class write_map>
class regex_node_or:public regex_node_base<write_map>
{
    typedef typename write_map::type_char char_type;
public:
    regex_node_base<write_map>* A;
    std::string name;
    regex_node_base<write_map>* B;
    regex_node_or(regex_node_base<write_map> *a,regex_node_base<write_map> *b)
        :A(a),B(b) {}
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *s=new write_map(name);
        A->write_to_map(a)->link(get_null_value<char_type>(),s);
        B->write_to_map(a)->link(get_null_value<char_type>(),s);
        return s;
    }
    virtual void write_last_name(std::string name)
    {
        this->name=name;
    }
    virtual void print(int tab=0)
    {
        for(int i=0; i<tab; i++)
        {
            printf(" ");
        }
        printf("regex_node_or\n");
        A->print(++tab);
        B->print(tab);
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
    typedef typename write_map::type_char char_type;
    std::string name;
    char_type left;
    char_type right;
public:
    regex_node_range(regex_node_char<write_map> *a,regex_node_char<write_map> *b):left(a->ch),right(b->ch)
    {
        delete a;
        delete b;
    }
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *s=new write_map(name);
        for(char_type ch=left; ch<=right; ch++)
        {
            a->link(ch,s);
        }
        return s;
    }
    virtual void write_last_name(std::string name)
    {
        this->name=name;
    }
    virtual void print(int tab=0)
    {
        for(int i=0; i<tab; i++)
        {
            printf(" ");
        }
        printf("regex_node_range:%c-%c\n",left,right);
    }
    ~regex_node_range()
    {
    }
};

template <class write_map>
class regex_node_one_none:public regex_node_base<write_map>
{
    typedef typename write_map::type_char char_type;
public:
    regex_node_base<write_map>* source;
    regex_node_one_none(regex_node_base<write_map>* source_):source(source_) {}
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *m=source->write_to_map(a);
        a->link(get_null_value<char_type>(),m);
        return m;
    }
    virtual void write_last_name(std::string name)
    {
        source->write_last_name(name);
    }
    virtual void print(int tab=0)
    {
        for(int i=0; i<tab; i++)
        {
            printf(" ");
        }
        printf("regex_node_one_none\n");
        source->print(++tab);
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
    typedef typename write_type::type_char char_type;
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
            b=new regex_node_or<write_type>(new regex_node_repeat<write_type>(b),new regex_node_char<write_type>(get_null_value<char_type>()));
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
            regex_node_char<write_type> *d=dynamic_cast<regex_node_char<write_type> *>(e);
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
        ++start;
        if(*start=='n')
            re=new regex_node_char<write_type>('\n');
        else if(*start=='t')
            re=new regex_node_char<write_type>('\t');
        else
            re=new regex_node_char<write_type>(*start);
        break;
    case '(':
        re=read_regex_string<write_type>(++start,finish);
        --start;
        break;
    case '^':
    {
        ++start;
        regex_node_base<write_type> *a=read_regex_unit<write_type>(start,finish);
        regex_node_char<write_type> *b=dynamic_cast<regex_node_char<write_type> *>(a);
        if(b==0)
        {
            throw a;
        }
        else
            re=new regex_node_not_char<write_type>(b->ch);
        start--;
    }
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
