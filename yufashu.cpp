#include <algorithm>
#include "yufashu.hpp"
#include <iterator>
using namespace yacc;

//lr(1)��Ŀ
class project
{
public:
	//����ʽ���
	input_type from_id;
	//����ʽ�Ҷ�
	const specification_left &to_id;
	//��Լ��������̷��ţ�
	input_type fin_id;
	//��ǰλ��
	size_t pos;
	project(input_type id_, const specification_left &to_id_, input_type fin_id_, size_t pos_ = 0) :
		from_id(id_), to_id(to_id_), fin_id(fin_id_), pos(pos_) {}
	bool operator==(const project &a)const
	{
		return &to_id == &a.to_id&&fin_id == a.fin_id&&pos == a.pos;
	}
	//��������ƶ���һ�����Ŀ
	project next() const
	{
		return project(from_id, to_id, fin_id, pos + 1);
	}
	//void print();
};
//hash��������Ҫ
namespace std
{
	template<> struct hash<project>
	{
		typedef project argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			result_type const h2(std::hash<input_type>{}(s.fin_id));
			result_type const h3(std::hash<const char*>{}((const char *)&s.to_id));
			result_type const h4(std::hash<size_t>{}(s.pos));
			return h2 ^ h3^h4;
		}
	};
}

//��ĿΪ״̬����
typedef project state_type;

gammer::gammer(state_map &&map_) :map(std::move(map_)){}
//֧�ֵ�����char��������ֵ�Ƚ��鷳ûŪ
enum v_type_n {
	float_, int_, char_
};
//������ϢposΪ����ջ��ƫ����typesΪ����
struct var_info
{
	size_t pos;
	v_type_n types;
};
void gammer::read_one(unit a)
{
	auto it = map.find(state_index(stack_state.back(), a.first));
	if (it != map.end())
	{
		std::visit([this, a](auto arg) {
			using T = std::decay_t<decltype(arg)>;
			//������ƽ�
			if constexpr (std::is_same_v<T, shift>)
			{
				stack_symbol.emplace_back(a.second);
				stack_state.emplace_back(arg.state);
			}
			//����ǹ�Լ
			else if constexpr (std::is_same_v<T, specification>)
			{
				any b = arg.func(stack_symbol.end() - arg.size);
				stack_symbol.erase(stack_symbol.end() - arg.size, stack_symbol.end());
				stack_state.erase(stack_state.end() - arg.size, stack_state.end());
				if (arg.id == root_id && a.first == fin_id)
				{
					stack_state.clear();
					stack_state.emplace_back(0);
					stack_symbol.clear();
				}
				else {
					read_one(unit(arg.id, b));
					read_one(a);
				}
			}
			else {
				assert("this varint must be specification or shift");
			}
		}, it->second);
	}
	else {
		printf("unexpect token %s\n", a.first.c_str());
		throw a;
	}
;
}
int bflag=0;
constexpr uint64_t flag = (std::numeric_limits<uint64_t>::max() ^ 0x1);
/*
�������Ҫ�Ǹ������ȼ������µ��ƽ��͹�Լ
����˵�������ƽ���Լ��ͻ��ʱ�����ȼ��ߵ�˵����
�������ڳ���ʵ�ֵ�ԭ�򣬹�Լ������ֱ��ʹ�õĹ�Լ����ʽ���ȼ�
�����ƽ�ʱ�޷���ȡ���ȼ���������ƽ�ʱֻ����״̬ת����Ϣ���������������һ��level_map�����Ӧ״̬ת�Ƶ����ȼ�
������������һ������level������ĳһ���ƽ������ȼ�
���ȼ���ϸ˵����specification_left��ע�����ȼ����ƱȽϸ��ӣ���Ҫ�뵱Ȼ����һ��˵��
*/
struct state_map
{
	std::unordered_map<state_index, op> map;
	//���浱ǰ������ȼ��Ƕ��ٺ��Ƿ����ƽ�
	std::unordered_map<state_index, std::pair<bool,size_t>> level_map;
	//�ƽ�
	void shift(size_t from, size_t to, input_type input) 
	{
		auto it = level_map.find(state_index(from, input));
		assert(it != level_map.end());
		if (it->second.first) {
			yacc::shift s;
			s.state = to;
			map.insert_or_assign(state_index(from, input), s);
			it->second.first = false;
		}
	}
	//��Լ
	void specification(size_t statue,const project& prj)
	{
		auto it = level_map.find(state_index(statue, prj.fin_id));
		//����߼��Ƚϸ��ӣ���Ҫ�ǱȽ����ȼ������ȼ���ϸ˵����specification_left��
		if (it == level_map.end() || prj.to_id.level <= (it->second.second&flag))
		{
			yacc::specification s;
			s.func = prj.to_id.handler;
			s.id = prj.from_id;
			s.size = prj.to_id.values.size();
			map.insert_or_assign(state_index(statue, prj.fin_id), s);
			level_map.insert_or_assign(state_index(statue, prj.fin_id), std::make_pair(false, prj.to_id.level));
		}
	}
	//�����ƽ����ȼ�
	void level(size_t from, input_type input,size_t lev)
	{
		auto it = level_map.find(state_index(from, input));
		if (it != level_map.end())
		{
			//��ǰ���Լ�������ƣ���Ҫ�ǱȽ����ȼ������ȼ���ϸ˵����specification_left��
			if ((lev&flag) < it->second.second)
			{
				it->second.first = true;
				it->second.second = lev;
			}
		}
		else {
			level_map.emplace(state_index(from, input), std::make_pair(true,lev));
		}
	}
};

std::ostream &operator << (std::ostream &o, project prj)
{
	o << prj.from_id << "<--";
	int n = prj.pos;
	for (auto a : prj.to_id.values)
	{
		if (n-- == 0) {
			o << ".";
		}
		else {
			o << " ";
		}
		o << a;
	}
	if (n == 0) {
		o << ".";
	}
	else {
		o << " ";
	}
	o << prj.fin_id;
	return o;
}
//��һ�����ʽת��Ϊ�﷨��������state_to_map���template.hpp
class grammer_maker :public state_to_map<state_type, input_type>
{
public:
	typedef typename state_to_map<state_type, input_type>::state_set state_set;
	typedef typename state_to_map<state_type, input_type>::next_map next_map;
	std::unordered_multimap<input_type, specification_left> gram_map;
	state_map map;
	grammer_maker() :now_level(0) {	}
	size_t now_level;
	///***
	//��first��
	void get_first_(input_type id, std::unordered_set<input_type> &set)
	{
		if (gram_map.find(id) == gram_map.end())
		{
			set.insert(id);
		}
		else
		{
			auto itp = gram_map.equal_range(id);
			bool flag = true;
			for (auto it = itp.first; it != itp.second; it++)
			{
				if (it->second.values.front() == id) {
					set.insert(id);
					continue;
				}
				else {
					get_first_(it->second.values.front(), set);
				}
			}
		}
	}
	void get_first(input_type id, std::unordered_set<input_type> &set)
	{
		get_first_(id, set);
	}
	///***
	//�հ�
	virtual void expand(state_type type, state_set &ptr)
	{
		if (type.pos != type.to_id.values.size())
		{
			input_type other = type.to_id.values[type.pos];
			auto itp = gram_map.equal_range(other);
			std::unordered_set<input_type> set;
			if (type.pos + 1 == type.to_id.values.size())
			{
				set.insert(type.fin_id);
			}
			else {
				get_first(type.to_id.values[type.pos + 1], set);
			}
			for (auto it = itp.first; it != itp.second; it++)
			{
				for (auto id : set)
				{
					project prd(it->first, it->second, id);
					if (ptr.find(prd) == ptr.end())
					{
						ptr.insert(prd);
						expand(prd, ptr);
					}
				}
			}
		}
	}
	///***
	//״̬ת��
	virtual void next(const state_set &ptr, next_map &map_)
	{
		size_t n = this->get_id(ptr);
		for (const state_type &a : ptr)
		{
			if (a.to_id.values.size() == a.pos)
			{
				map.specification(n, a);
			}
			else
			{
				auto b = a.to_id.values[a.pos];
				map_[b].insert(a.next());
				map.level(n, b, a.to_id.level);
			}
		}
	}
	///***
	//�ƽ�
	virtual void link(const state_set &from_set, const state_set &to_set, input_type input)
	{
		map.shift(this->get_id(from_set), this->get_id(to_set), input);
	}

	virtual void add_state(size_t n, const state_set &ptr) {
	}
	///***
	//���һ������ʽ����handle��Ϊ��Լ����
	specification_left &add(input_type a)
	{
		specification_left l;
		l.level = now_level++ << 1;
		return gram_map.emplace(a, l)->second;
	}
	///***
	//�����﷨������
	std::shared_ptr<gammer> make_grammer(input_type root_state, specification_handle root_handle)
	{
		specification_left l;
		l.values.emplace_back(root_state);
		l.handler = root_handle;
		auto b = gram_map.emplace(root_id, l);
		this->make_map(project(std::string(root_id), b->second, std::string(fin_id)));
		gram_map.clear();
		now_level = 0;
		map.level_map.clear();
		return std::make_shared<gammer>(std::move(map.map));
	}
};

//���涼����Ҫ
thread_local grammer_maker global_grammer_impl;
thread_local size_t sid = 1;

std::shared_ptr<gammer> yacc::make_grammer(symbol_impl sym, specification_handle root_handle)
{
	sid = 1;
	return global_grammer_impl.make_grammer(sym.id, root_handle);
}


symbol_impl::symbol_impl(std::string name):id(name)
{
}

specification_left &symbol_impl::operator=(const std::initializer_list<symbol_impl> &symbols)
{
	auto &l = global_grammer_impl.add(id);;
	std::vector<input_type> vec(symbols.size());
	l.values.reserve(symbols.size());
	std::transform(symbols.begin(), symbols.end(), std::back_inserter(l.values), [](auto a) {return a.id; });
	return l;
}

bool symbol_impl::operator==(const symbol_impl &o) const
{
	return id==o.id;
}

specification_left &yacc::specification_left::operator,(pass_by p)
{
	
	if (p.n == -1) {
		handler = [](unit_it b) {return not_use(); };
	}
	else {
		handler = [a = p.n](unit_it b){return b[a]; };
	}
	return *this;
}

specification_left &yacc::specification_left::operator,(right_combin)
{
	level |= 0x1;
	return *this;
}

std::shared_ptr<yacc::gammer> yacc::make_grammer(symbol_impl sym, std::function<void(not_use)> root_handle)
{
	return make_grammer(sym,(yacc::specification_handle) [](unit_it a) {return not_use(); });
}

#include <time.h>
//һֱ������λ�ö�����Ҫ
//ԭ�������õ����ڵ�������
//��ϸ�÷��뿴main.cpp
int main__() {
	//�﷨�������÷�
	//�����ķ�����
	symbol(a);
	symbol(b);
	symbol(c);
	symbol(d);
	bflag = 1;
	//�������ʽ�������lambda���ʽ�൱�ڶ�̬����ĺ���������ʽ��Լʱ���ᱻ����
	//Ϊʲô����д������������Ϊ�������˸�ֵ������Ͷ��������
	a = { a,b ,a}, [](not_use) {
		printf("ab\n");
		return not_use();
	};
	a = { a,c,a }, [](not_use) {
		printf("ca\n");
		return not_use();
	};
	a = { d }, [](not_use) {
		printf("d\n");
		return not_use();
	};
	//�����﷨������
	std::shared_ptr<gammer> asd = make_grammer(a, [](not_use a) {}); {
	}
	std::vector<unit> vec;
	vec.push_back(std::make_pair(d.id, std::any(1)));
	vec.push_back(std::make_pair(c.id, std::any(1)));
	vec.push_back(std::make_pair(d.id, std::any(1)));
	vec.push_back(std::make_pair(b.id, std::any(1)));
	vec.push_back(std::make_pair(d.id, std::any(1)));
	//�����������Ϊ�������d\nca\nab\n
	asd->read(vec.begin(), vec.end());

	return 0;
}

yacc::pass_by::pass_by(size_t m):n(m)
{
}
