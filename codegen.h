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
		bool is_interger();
		bool is_function();
		std::variant<std::wstring, function>&father();
		bool operator == (const base_type&o)const;
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
		std::vector<std::pair<type,std::wstring>> arg_type;
		bool have_finish;
		operator std::wstring();
		function_();
		std::wstring to_function_dec(std::wstring name);
		std::wstring to_function_call();
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