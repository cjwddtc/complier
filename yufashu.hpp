#include <map>
#include <vector>
#include <array.h>

template <class id_type>
class production{
    id_type from_id;
    std::vector<id_type> to_id;
}

template <class id_type>
class grammar{
    std::vector<production> productions;
    std::set<id_type> fin_id_set;
    id_type max_id;
    bool is_fin_id(id_type a){
        return fin_id_set.find(a)!=a.end()
    }
}

template <class id_type>
class project{
    production<id_type> pro;
    size_t pos;
}

template <class id_type>
class op{
    
}

template <class id_type>
class parser{
    std::vector<array<op<id_type>>> map;
}