#ifndef REGEX_HPP_INCLUDED
#define REGEX_HPP_INCLUDED
#include <list>

template <class write_map>
class regex_node_base
{
public:
    virtual ~regex_node_base()=default;
<<<<<<< HEAD
    typedef typename write_map::type_char char_type;
    //this func is to convert regex to map by recurse
    //the b is input state return the state after the func called
    virtual write_map *write_to_map(write_map *a)=0;
    virtual void write_last_name(std::string name)=0;
    virtual void print(int tab=0)=0;
=======
    typedef typename write_map::type_state state_type;
    //this func is to convert regex to map by recurse
    //the b is input state return the state after the func called
    virtual state_type &write_to_map(write_map &a,state_type &b)=0;
>>>>>>> github/master
};

template <class write_map>
class regex_node_char:public regex_node_base<write_map>
{
<<<<<<< HEAD
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
    virtual void write_last_name(std::string name){
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
        for(char_type ch=get_range_low<char_type>();ch<get_range_hight<char_type>();ch++)
            if(ch!=this->ch)
                a->link(ch,s);
        return s;
    }
    virtual void write_last_name(std::string name){
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
=======
    typedef typename regex_node_base<write_map>::state_type state_type;
    typedef typename write_map::value_type line;
public:
    char ch;
    regex_node_char(char ch_)
    {
        ch=ch_;
    }
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {/*
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
        return a[b][ch];*/
    }
    virtual ~regex_node_char()=default;
>>>>>>> github/master
};

template <class write_map>
class regex_node_block:public regex_node_base<write_map>
{
<<<<<<< HEAD
    typedef typename write_map::type_char char_type;
=======
    typedef typename regex_node_base<write_map>::state_type state_type;
>>>>>>> github/master
public:
    std::list<regex_node_base<write_map>*> regex_nodes;
    void add(regex_node_base<write_map> *a)
    {
        regex_nodes.push_back(a);
    }
<<<<<<< HEAD
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *ptr=a;
        for(regex_node_base<write_map>* b:regex_nodes){
            ptr=b->write_to_map(ptr);
        }
        return ptr;
    }
    virtual void write_last_name(std::string name){
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
=======
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        state_type *ptr=&b;
        for(regex_node_base<write_map>* c:regex_nodes)
        {
            ptr=&c->write_to_map(a,*ptr);
        }
        return *ptr;
>>>>>>> github/master
    }
    virtual ~regex_node_block()
    {
        for(regex_node_base<write_map> *a:regex_nodes)
        {
            delete a;
        }
    }
};
<<<<<<< HEAD
//重复任意次 至少一次
template <class write_map>
class regex_node_repeat:public regex_node_base<write_map>
{
    typedef typename write_map::type_char char_type;
=======

template <class write_map>
class regex_node_repeat:public regex_node_base<write_map>
{
    typedef typename regex_node_base<write_map>::state_type state_type;
>>>>>>> github/master
public:
    regex_node_base<write_map> *repeat_source;
    regex_node_repeat(regex_node_base<write_map> *a):
        repeat_source(a) {}
<<<<<<< HEAD
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *b=new write_map();
        write_map *c=repeat_source->write_to_map(b);
        c->link(get_null_char<char_type>(),b);
        a->link(get_null_char<char_type>(),b);
        return c;
    }
    virtual void write_last_name(std::string name){
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
=======
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        state_type &c=repeat_source->write_to_map(a,b);

>>>>>>> github/master
    }
    virtual ~regex_node_repeat()
    {
        delete repeat_source;
    }
};

template <class write_map>
class regex_node_or:public regex_node_base<write_map>
{
<<<<<<< HEAD
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
        A->write_to_map(a)->link(get_null_char<char_type>(),s);
        B->write_to_map(a)->link(get_null_char<char_type>(),s);
        return s;
    }
    virtual void write_last_name(std::string name){
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
=======
    typedef typename regex_node_base<write_map>::state_type state_type;
public:
    regex_node_base<write_map>* A;
    regex_node_base<write_map>* B;
    regex_node_or(regex_node_base<write_map> *a,regex_node_base<write_map> *b)
        :A(a),B(b) {}
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        std::cout << "regex_node_or" << std::endl;
>>>>>>> github/master
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
<<<<<<< HEAD
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
    virtual void write_last_name(std::string name){
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
=======
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
>>>>>>> github/master
    }
};

template <class write_map>
class regex_node_one_none:public regex_node_base<write_map>
{
<<<<<<< HEAD
    typedef typename write_map::type_char char_type;
public:
    regex_node_base<write_map>* source;
    regex_node_one_none(regex_node_base<write_map>* source_):source(source_) {}
    virtual write_map *write_to_map(write_map *a)
    {
        write_map *m=source->write_to_map(a);
        a->link(get_null_char<char_type>(),m);
        return m;
    }
    virtual void write_last_name(std::string name){
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
=======
    typedef typename regex_node_base<write_map>::state_type state_type;
public:
    regex_node_base<write_map>* source;
    regex_node_one_none(regex_node_base<write_map>* source_):source(source_) {}
    virtual state_type &write_to_map(write_map &a,state_type &b)
    {
        std::cout << "regex_node_one_none" << std::endl;
>>>>>>> github/master
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
<<<<<<< HEAD
    typedef typename write_type::type_char char_type;
=======
>>>>>>> github/master
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
<<<<<<< HEAD
            regex_node_base<write_type> *&b=a->regex_nodes.back();
            b=new regex_node_or<write_type>(new regex_node_repeat<write_type>(b),new regex_node_char<write_type>(get_null_char<char_type>()));
=======
            regex_node_base<write_type> *b=a->regex_nodes.back();
            a->add(new regex_node_repeat<write_type>(b));
>>>>>>> github/master
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
<<<<<<< HEAD
            regex_node_char<write_type> *d=dynamic_cast<regex_node_char<write_type> *>(e);
=======
            regex_node_char<write_type> *d=dynamic_cast<regex_node_char<write_type> *>(b);
>>>>>>> github/master
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
<<<<<<< HEAD
        ++start;
        if(*start=='n')
            re=new regex_node_char<write_type>('\n');
        else if(*start=='t')
            re=new regex_node_char<write_type>('\t');
        else
            re=new regex_node_char<write_type>(*start);
=======
        re=new regex_node_char<write_type>(*++start);
>>>>>>> github/master
        break;
    case '(':
        re=read_regex_string<write_type>(++start,finish);
        --start;
        break;
<<<<<<< HEAD
    case '^':
        {
            ++start;
            regex_node_base<write_type> *a=read_regex_unit<write_type>(start,finish);
            regex_node_char<write_type> *b=dynamic_cast<regex_node_char<write_type> *>(a);
            if(b==0){
                throw a;
            }else
                re=new regex_node_not_char<write_type>(b->ch);
            start--;
        }
        break;
=======
>>>>>>> github/master
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
