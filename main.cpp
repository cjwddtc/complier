#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <assert.h>
#include "predef.h"
#include "cffx.hpp"
#include "regex_map_two.hpp"
using std::string;
typedef map_two<state_line<int,char>> map_two_de;
int main()
{
    auto_machin<int,char> b;
    string str="a+(s*|d)?|f";
    auto sta=str.begin();
    regex_node_block<map_two_de> *q=read_regex_string<map_two_de>(sta,str.end());
    return 0;
}
