#include <string>
#include <map>
#include <vector>
#include <optional>
#include <variant>
#include <ostream>
namespace codegen {

	struct function_;
	typedef std::shared_ptr<function_> function_type;
	enum value_type
	{
		interger,function,number,pointer,array
	};

	typedef  std::variant<std::wstring, function_type> base_type;

	struct type_info
	{
		base_type b_type;
		std::wstring &t_type();
		function_type f_type();
		value_type type_type()const ;
		std::vector<size_t> plus;
		bool operator==(const type_info &a) const;
		bool is_void_ptr();
		bool is_void();
	};


	int cmp_type(base_type a, base_type b);


	struct function_
	{
		type_info ret_type;
		std::vector<type_info> arg_type;
		bool have_finish;
		function_();
		bool operator==(const function_ &)const;
	};
	
	struct addr_var;

	struct tmp_var
	{
		std::wstring real_name;
		type_info type_;
		addr_var deref();
		addr_var ptr_off_set(tmp_var var);
	};

	tmp_var convert(tmp_var s, type_info t, bool is_force);
	void same_var(tmp_var &s, tmp_var &t);

	struct addr_var
	{
		std::wstring real_name;
		type_info type_;
		tmp_var load();
		void save(tmp_var var);
		tmp_var addr();
		tmp_var call(std::vector<tmp_var> &var);
		void func_sign(std::vector<std::wstring> names);
		addr_var off_set(size_t index);
		addr_var array_off_set(size_t index);
	};

	void inst(tmp_var a, tmp_var b, std::wstring ins);

	struct name_space
	{
		std::map<std::wstring, addr_var> global;
		size_t global_index;
		std::optional<std::pair<size_t, std::map<std::wstring, addr_var> > >  maps;
		addr_var &find(std::wstring str);
		addr_var &add(std::wstring name);
		tmp_var newvar();
		name_space();
		void enable(bool flag);
	};

	std::wostream &operator<<(std::wostream &o, codegen::type_info t);
	std::wostream &operator<<(std::wostream &o, codegen::base_type &t);
	std::wostream &operator<<(std::wostream &o, codegen::function_type &t);
	std::wostream &operator<<(std::wostream &o, addr_var &t);
	std::wostream &operator<<(std::wostream &o, tmp_var &t);
	extern name_space map;
}