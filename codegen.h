#include <string>
#include <map>
#include <vector>
#include <optional>
#include <variant>
namespace codegen {

	struct function;
	struct type
	{
		std::variant<std::wstring,function> base_type;
		std::vector<size_t> plus;
		operator std::wstring();
		bool is_variable();
		size_t size();
		bool operator==(const type &a) {
			return base_type == a.base_type && plus == a.plus;
		}
	};

	struct function
	{
		type ret_type;
		std::vector<type> arg_type;
		bool have_finish;
		operator std::wstring();
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
		name_space();
		void enable(bool flag);
	};

}