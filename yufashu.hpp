//�﷨������
#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <assert.h>
#include <set>
#include <stack>
#include "template.hpp"
#include <string>
#include <variant>
#include <functional>
#include <any>

//lr1��Ŀ
namespace yacc {
	using ::std::pair;
	using ::std::vector;
	using ::std::function;
	using ::std::any;
	using ::std::variant;
	//����Ϊ�ķ�����id
	typedef size_t input_type;
	//����ʽ�Ҳ���﷨�Ƶ�����ĺ���
	//ÿ���ķ����Ŷ���һ��ֵ�����﷨�Ƶ������������Ͳ�ͬ��Ϊany
	typedef pair<const size_t, any> unit;
	typedef vector<any>::iterator unit_it;
	//��Լʱ�Ĵ�������
	typedef function<any(unit_it start)> specification_handle;
	//һ������ʽ�����
	typedef pair<vector<input_type>, specification_handle> specification_left;
	//�ƽ�
	struct shift
	{
		size_t state;
	};
	//��Լ
	struct specification
	{
		size_t size;
		input_type id;
		specification_handle func;
	};
	//�ƽ����ǹ�Լ
	typedef variant<shift, specification> op;
	//״̬ת�Ʊ����������ǰ״̬�������ķ�����
	typedef pair<size_t, size_t> state_index;
	//�����಻��Ҫ
	struct not_use {};
}

//ʵ��hash����������Ҫ
namespace std
{
	template<> struct hash<yacc::state_index>
	{
		typedef yacc::state_index argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			result_type const h1(std::hash<size_t>{}(s.first));
			result_type const h2(std::hash<size_t>{}(s.second));
			return h1 ^ h2;
		}
	};
}
//������ͨ��һЩ�漼���ɼ򻯴����д������Ҫ
namespace tmp
{
	using std::any;
	template <class T, class F>
	any invoke(std::function < T(F) > func, yacc::unit_it a)
	{
		return func(std::any_cast<F>(a[0]));
	}
	template <class T>
	any invoke(std::function < T(yacc::not_use) > func, yacc::unit_it a)
	{
		return func(yacc::not_use());
	}
	template <class T,class ...ARG>
	any invoke(std::function < T(yacc::not_use,ARG...) > func, yacc::unit_it a)
	{
		std::function<T(ARG...)> f = [func, a](ARG ... arg) {return func(yacc::not_use(), arg...);};
		return invoke(f, a + 1);
	}
	template <class T, class F, class ...ARG>
	any invoke(std::function < T(F, ARG...) > func, yacc::unit_it a)
	{
		std::function<T(ARG...)> f = [func, a](ARG ... arg) {return func(std::any_cast<F>(a[0]), arg...); };
		return invoke(f, a + 1);
	}
	template<typename T>
	struct memfun_type
	{
		using type = void;
	};
	template<typename Ret, typename Class, typename... Args>
	struct memfun_type<Ret(Class::*)(Args...) const>
	{
		using type = std::function<Ret(Args...)>;
	};
	template<typename F>
	typename memfun_type<decltype(&F::operator())>::type
		FFL(F const &func)
	{//Function from lambda!
		return func;
	}
}
namespace yacc{
	//�ʷ�������
	class gammer
	{
		typedef std::unordered_map<state_index, op> state_map;
		//״̬ת�Ʊ�
		state_map map;
		//����ջ����ÿ���ķ����ŵ�����any���Դ�����������
		std::vector<any> stack_symbol;
		//״̬ջ
		std::vector<size_t> stack_state;
	public:
		gammer(state_map &&map_);
		//��ȡһ������
		void read_one(unit a);
		//��ȡ��start��end�ĵ�Ԫ����ĩβ��һ��������
		template <class ITERATOR>
		void read(ITERATOR start, ITERATOR end)
		{
			stack_state.clear();
			stack_symbol.clear();
			stack_state.emplace_back(0);
			while (start != end)
			{
				read_one(*start++);
			}
			read_one(std::make_pair(-1, std::any()));
		}
	};
	//�������ʾ��ι�Լֻ�ǵ����Ľ���n������Լ���ŵ�������Ϊ��Լ�ɵ����ķ����ŵ�����
	struct pass_by
	{
		size_t n;
		pass_by(size_t m);
	};
	//����������ʵ�������д��
	class binder
	{
		std::vector<input_type> symbols;
		input_type id;
		void add(specification_handle han);
	public:
		binder(input_type id_, std::vector<input_type> &&syms);
		void operator,(pass_by p);
		template <class T>
		void operator,(T han)
		{
			add([han](unit_it a) {return tmp::invoke(tmp::FFL(han), a); });
		}
	};
	//�ķ�����
	struct symbol
	{
		size_t id;
		symbol();
		//ͨ����ֵ�������ʾһ������ʽ�����÷���cpp�ļ�ĩβ��main_�ļ�
		binder operator=(std::initializer_list<symbol> symbols);
		bool operator==(const symbol &)const;
	};
	//����ȫ������һЩ������漼���ɣ�����Ҫ����Ҫ�Ǹ����û�����ĺ����Զ��Ľ���Ӧ�����ʹ�any��ȡ��
	std::shared_ptr<gammer> make_grammer(symbol sym, specification_handle root_handle);
	template <class T>
	std::shared_ptr<gammer> make_grammer(symbol sym, std::function<void(T)> root_handle)
	{
		specification_handle b = [root_handle](unit_it i) {
			root_handle(std::any_cast<T>(*i));
			return any();
		};
		return make_grammer(sym, b);
	}
	std::shared_ptr<gammer> make_grammer(symbol sym, std::function<void(not_use)> root_handle);
	template <class T>
	std::shared_ptr<gammer> make_grammer(symbol sym, T root_handle)
	{
		return make_grammer(sym,tmp::FFL(root_handle));
	}
}
