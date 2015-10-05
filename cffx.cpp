#include <iostream>
template <class state_type,class code_tpye>
class print_
{
public:
    void operator()(std::basic_string<code_tpye> a,state_type b)
    {
        std::cout << a << "\n" << b << std::endl;
    }
};
