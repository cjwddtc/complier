#ifndef ID_MANAGER_H_INCLUDED
#define ID_MANAGER_H_INCLUDED
#include <map>
#include <vector>

class id_manager
{
    std::map<std::string,size_t> name_map;
    std::vector<std::string> id_map;
    size_t last_id;
public:
    size_t start_id;
    size_t get_id(std::string str);
    size_t get_new_id();
    size_t get_front_id();
    std::string get_name(size_t id);
    id_manager();
};


#endif // ID_MANAGER_H_INCLUDED
