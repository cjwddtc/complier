#include <string>
#include <map>
#include <vector>
#include <optional>
namespace codegen {
	struct type
	{
		std::wstring base_type;
		std::vector<size_t> plus;
		operator std::wstring();
		bool is_array();
		size_t size();
	};


	struct var
	{
		std::wstring real_name;
		type type;
	};

	struct name_space
	{
		std::map<std::wstring, var> global;
		size_t global_index;
		std::optional<std::pair<size_t, std::map<std::wstring, var> > >  maps;
		var &find(std::wstring str);
		var &add(std::wstring name);
		var newvar();
		void enable(bool flag);
	};

}