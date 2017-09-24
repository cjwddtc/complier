#include "id_manager.h"
#include <string>
id_manager::id_manager():start_id(0),last_id(0) {}
size_t id_manager::get_id(std::string str)
{
    if(name_map.find(str)==name_map.end())
    {
        name_map.insert(make_pair(str,start_id++));
        id_map.push_back(str);
    }
    last_id=name_map[str];
    return name_map[str];
}

std::string id_manager::get_name(size_t id)
{
    return id_map[id];
}


size_t id_manager::get_front_id()
{
    return last_id;
}

size_t id_manager::get_new_id()
{
    last_id=start_id;
    return start_id++;
}
