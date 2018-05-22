#include "dfa.h"
#include <algorithm>
using output_type=yacc::input_type;
class nfa
{
	//nfa��״̬ת�Ƶ��������˵��optional��ʾ���԰���wchar_tҲ����Ϊ�գ�Ϊ��ʱ��ʾΪ�ձߣ�����ֱ��ת����
	typedef std::optional<wchar_t> input_type;
	//ĳһ��nfa״̬�������еı�,size_tΪnfa״̬�������е�����
	typedef std::unordered_multimap<input_type, size_t> edge_map;
	//���optional��ʾ��״̬�Ƿ�Ϊ�ɽ���״̬������Ǵ�optional�������ʱ����Ĵʷ���Ԫid
	//�����������һ����Ϊnfa��״̬
	typedef std::pair<edge_map, std::optional<output_type>> nfa_status;
	//��ǰ�����id,����nfa��ʹ��
	output_type c_id;
	//nfa״̬����
	std::unordered_map<wchar_t, symbol_impl> id_map;
	//����������ʽ���﷨������
	std::shared_ptr<grammer> reggm;
	//����һ��nfa״̬�������������е�����
	size_t create_status()
	{
		status.push_back(nfa_status());
		return status.size() - 1;
	}
	//����һ��nfa״̬Ϊ�ɽ���״̬,����ʱ���λname����״̬�������е�����Ϊid
	void set_name(size_t id, output_type name)
	{
		status[id].second = name;
	}
	//���һ��nfa��,����������Ĭ��Ϊ�ձ�
	void link(size_t from, size_t to, input_type ch = input_type())
	{
		status[from].first.emplace(ch, to);
	}
protected:
	//nfa״̬����
	std::vector<nfa_status> status;
public:
	nfa();
	//��һ��������ʽ��ӵ�nfa��
	void add(std::wstring str, std::string id);
};
namespace std
{
	//hash��������Ҫ
	hash<status_index>::result_type hash<status_index>::operator()(argument_type const& s) const noexcept
	{
		result_type const h1(std::hash<size_t>{}(s.first));
		result_type const h2(std::hash<size_t>{}(s.second));
		return h1 ^ h2;
	}
}
using yacc::unit_it;
using yacc::unit;
typedef std::pair<size_t, size_t> node_s;
using yacc::pass_by;
//���������Ҫ�����﷨����������������ʽ,�﷨������ʹ�÷�������main.cpp������
nfa::nfa()
{
	//��������Ҫ�Ǳܿ�0״̬��Ϊ��ʼ״̬
	create_status();
	symbol(char_);//regularchar
	symbol(minus);//-
	symbol(left_small);//(
	symbol(right_small);//)
	symbol(star);//*
	symbol(plus);//+
	symbol(question);//?
	symbol(backslash);//��б��
	symbol(or_) ;//|
	symbol(exp);//(a)
	id_map.emplace(L'(',left_small);
	id_map.emplace(L')', right_small);
	id_map.emplace(L'*', star);
	id_map.emplace(L'+', plus);
	id_map.emplace(L'-', minus);
	id_map.emplace(L'?', question);
	id_map.emplace(L'\\', backslash);
	id_map.emplace(L'|', or_);
	using yacc::not_use;
	for (auto &b : id_map)
	{
		exp = { backslash,b.second }, [this](not_use,wchar_t ch) {
			size_t start_s = create_status();
			size_t end_s = create_status();
			link(start_s, end_s,ch);
			return std::make_pair(start_s, end_s);
		};
	}
	exp = { backslash,char_}, [this](not_use ,wchar_t ch) {
		size_t start_s = create_status();
		size_t end_s = create_status();
		link(start_s, end_s, ch);
		return std::make_pair(start_s, end_s);
	};
	exp = { char_,minus,char_ }, [this](wchar_t a,not_use,wchar_t b)
	{
		size_t start_s = create_status();
		size_t end_s = create_status();
		while (a <= b)
		{
			link(start_s, end_s, a++);
		}
		return std::make_pair(start_s, end_s);
	};
	exp = { exp,star }, [this](node_s a, not_use) {
		link(a.second, a.first);
		link(a.first, a.second);
		return a;
	};
	exp = { exp,plus }, [this](node_s a, not_use) {
		link(a.second, a.first);
		return a;
	};
	exp = { exp,question }, [this](node_s a, not_use) {
		link(a.first, a.second);
		return a;
	};
	exp = { exp,exp }, [this](node_s a, node_s b) {
		link(a.second, b.first);
		return std::make_pair(a.first, b.second);
	};
	exp = { left_small ,exp,right_small }, pass_by(1);
	exp = { char_ }, [this](wchar_t ch) {
		size_t start_s = create_status();
		size_t end_s = create_status();
		link(start_s, end_s,ch);
		return std::make_pair(start_s, end_s);
	};
	exp = { exp,or_,exp }, [this](node_s a,not_use,node_s b) {
		link(a.first, b.first);
		link(b.second, a.second);
		return a;
	};
	
	reggm = make_grammer(exp,[this](node_s b) {
		link(0, b.first);
		set_name(b.second, c_id);
	});
}
const std::string char_string = "char_";
void nfa::add(std::wstring str, std::string id)
{
	std::vector<unit> m;
	m.reserve(str.size());
	std::transform(str.begin(), str.end(),std::back_inserter(m), [this](wchar_t ch) {
		auto it = id_map.find(ch);
		if (it != id_map.end())
		{
			return unit(it->second.id, std::any(ch));
		}
		else {
			return unit("char_", std::any(ch));
		}
	});
	c_id=id;
	reggm->read(m.begin(), m.end());
}
//nfaתdfa�࣬�ǳ����ӣ�һ����̳�����Ҫת����nfa
//����һ���漯����һ���������,����ͨ������һ����������д�ĺ�����ʵ�ֹ���״̬ת�Ʊ�Ĺ���
//�����뿴template.hpp
//ֻ��Ҫʵ��Ԥ����õıհ���expand����״̬ת�ƣ�next���������������״̬����ת�Ƶ�������״̬���ȿ���ʵ��nfaתdfa
class dfa_maker :public state_to_map<state_type, input_type>, public nfa
{
public:
	typedef typename state_to_map<state_type, wchar_t>::state_set state_set;
	typedef typename state_to_map<state_type, wchar_t>::next_map next_map;
	std::unordered_map<status_index, size_t> status_map;
	std::unordered_map<state_type, output_type> fin_status;
	///***
	virtual void expand(state_type id, state_set &ptr)
	{
		auto &stat = status[id].first;
		auto b = stat.equal_range(std::optional<wchar_t>());
		while (b.first != b.second)
		{
			if (ptr.find(b.first->second) == ptr.end())
			{
				ptr.emplace(b.first->second);
				expand(b.first->second, ptr);
			}
			++b.first;
		}
	}
	///***
	virtual void next(const state_set &ptr, next_map &map)
	{
		for (size_t b : ptr)
		{
			for (auto a : status[b].first)
			{
				if (a.first) {
					map[*a.first].insert(a.second);
				}
			}
		}
	}
	///***
	//state_to_map�ڴ���һ���±ߵ�ʱ�����
	virtual void link(const state_set &from_set, const state_set &to_set, wchar_t ch)
	{
		status_map[std::make_pair(this->get_id(from_set), ch)] = this->get_id(to_set);
	}
	///***
	//state_to_map���½�״̬�ǵ���
	virtual void add_state(size_t n, const state_set &ptr)
	{
		size_t maxd=-1;
		//�ж����״̬�Ƿ��ǿɽ���״̬��
		for (size_t d : ptr)
		{
			if (status[d].second)
			{
				if (d < maxd) {
					fin_status[n] = *status[d].second;
					maxd = d;
				}
			}
		}
	}
	//���ø���make_map nfaתdfa
	std::shared_ptr<dfa> make_dfa()
	{
		make_map(0);
		return std::make_shared<dfa>(std::move(status_map), std::move(fin_status));
	}
};
//ȫ�ֵ�һ��dfa_maker
thread_local dfa_maker global_dfa;

std::shared_ptr<dfa> make_dfa()
{
	return global_dfa.make_dfa();
}

final_symbol_impl::final_symbol_impl(std::wstring str,std::string id):symbol_impl(id)
{
	global_dfa.add(str, this->id);
}

dfa::dfa(std::unordered_map<status_index, size_t>&& m, std::unordered_map<state_type, output_type>&& f)
	:status_map(std::move(m)),fin_status(std::move(f))
{
}

null_symbol_impl::null_symbol_impl(std::wstring str):symbol_impl(std::string(ignore_id))
{
	global_dfa.add(str, std::string(ignore_id));
}

force_init::force_init()
{
	&global_dfa;
}