#include <string>
#include <map>
#include <vector>
#include <optional>
#include <variant>
namespace codegen {

	struct function_;
	typedef std::shared_ptr<function_> function;
	struct base_type :public std::variant<std::wstring, function>
	{
		bool is_interger() {
			return true;
		}
		bool is_function() {
			return std::visit([](auto arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, function>)
				{
					return false;
				}
				else {
					return true;
				}
			},father());
		}
		std::variant<std::wstring, function>&father()
		{
			return (std::variant<std::wstring, function> &)*this;
		}
	};
	struct type
	{
		base_type base_type;
		std::vector<size_t> plus;
		bool operator==(const type &a) const;
		operator std::wstring();
		bool is_variable();
		size_t size();
	};

	struct function_
	{
		type ret_type;
		std::vector<type> arg_type;
		bool have_finish;
		operator std::wstring();
		std::wstring to_string(std::wstring name);
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