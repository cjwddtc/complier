#include <string>
#include <map>
#include <vector>
struct type
{
	std::wstring base_type;
	std::vector<size_t> plus;
	operator std::string();
	bool is_array();
};


struct var
{
	std::wstring real_name;
	type type;
};

struct name_space
{
	std::map<std::wstring, var> global;
};