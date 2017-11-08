#include <vector>
#include <tuple>
#include <functional>
#include <tuple>
//typedef std::vector<std::tuple<uint16_t,uint16_t,char>> nfa_type;

//typedef std::map<uint16_t,std::multimap<char,uint16_t>> nfa_type;

//a normal char
constexpr auto _ch(char ch)
{
	return std::make_tuple(1,0,[ch](uint16_t sta,uint16_t &id,auto &edges,auto &zero_edges) {
		edges[sta].first=id;
		edges[sta].second=ch;
		return id++;
	});
}

//a or b
constexpr auto _or(auto a, auto b)
{
	return std::make_tuple(
			std::get<0>(a)+std::get<0>(b),
			std::get<1>(a)+std::get<1>(b)+1,
			[a=std::get<2>(a),b=std::get<2>(a)](uint16_t sta,uint16_t &id, auto &edges,auto &zero_edges) {
		auto i=b(sta,id,edges,zero_edges);
		zero_edges.second[zero_edges.first].first=a(sta,id,edges,zero_edges);
		zero_edges.second[zero_edges.first].second=i;
		++zero_edges.first;
		return i;
	});
}

//a str use '',len of the str must be small than 8
constexpr auto _str(uint64_t str)
{
	return std::make_tuple(8,0,[str](uint16_t sta,uint16_t &id, auto &edges,auto &zero_edges) {
		uint8_t *p=(uint8_t *)&str;
		for(int i=7;i>=0;i--){
			edges[sta].first=id;
			edges[sta].second=p[i];
			sta = id++;
		}
		return sta;
	});
}


//from a to b
constexpr auto _range(char a,char b)
{
	return std::make_tuple(1,0,[a,b](uint16_t sta,uint16_t &id, auto &edges,auto &zero_edges) {
		for(char i=a;i!=b;i++)
		{
			edges[sta].first=id;
			edges[sta].second=i;
		}
		return id++;
	});
}

//not ch
constexpr auto _not(char ch)
{
	return std::make_tuple(1,0,[ch](uint16_t sta,uint16_t &id,auto &edges,auto &zero_edges) {
		for(char i=0;i!=-1;i++)
		{
			if(i!=ch){
				edges[sta].first=id;
				edges[sta].second=i;
			}
		}
		//a.emplace_back(sta, word_id, ch);
		return id++;
	});
}

//a repeat zero to one times
constexpr auto _zo(auto a)
{
	return std::make_tuple(std::get<0>(a),std::get<1>(a)+1,
			[a=std::get<2>(a)](uint16_t sta,uint16_t &id,auto &edges,auto &zero_edges) {
		zero_edges.second[zero_edges.first].first=sta;
		auto i=a(sta,id,edges,zero_edges);
		zero_edges.second[zero_edges.first].second=i;
		++zero_edges.first;
		return i;
	});
}

//a repeat one to infinit times
constexpr auto _oi(auto a)
{
	return std::make_tuple(std::get<0>(a),std::get<1>(a)+1,
			[a=std::get<2>(a)](uint16_t sta,uint16_t &id,auto &edges,auto &zero_edges) {
		auto i=a(sta,id,edges,zero_edges);
		zero_edges.second[zero_edges.first].first=i;
		zero_edges.second[zero_edges.first].second=sta;
		++zero_edges.first;
		return i;
	});
}

//a can be repeat any times inlucde zero
constexpr auto zi(auto a)
{
	return std::make_tuple(std::get<0>(a),std::get<1>(a)+1,
			[a=std::get<2>(a)](uint16_t sta,uint16_t &id,auto &edges,auto &zero_edges) {
		zero_edges.second[zero_edges.first].first=a(sta,id,edges,zero_edges);
		zero_edges.second[zero_edges.first].second=sta;
		++zero_edges.first;
		return sta;
	});
}

constexpr auto sort(auto array,auto com)
{
	decltype(array) new_array;
	for(int i=0;i<array.size();i++)
	{
		//new_array[i]=array[i]
		new_array[i].first=array[i].first;
		new_array[i].second=array[i].second;
		for(int j=i+1;j<array.size();j++)
		{
			if(com(array[j],new_array[i]))
			{
				new_array[i]=array[j];
			}
		}
	}
	return new_array;
}

constexpr auto extend(auto e,auto ze)
{
	return sizeof(e)+sizeof(ze);
}

constexpr auto run(auto b){
	constexpr auto a=b();
	//constexpr auto a=_or(_ch('a'),_ch('b'));
	std::array<std::pair<uint16_t,char>,std::get<0>(a)> edges; 
	std::pair<uint32_t,std::array<std::pair<uint16_t,uint16_t>,std::get<1>(a)>> zero_edges; 
	uint16_t id=1;
	std::get<2>(a)(0,id,edges,zero_edges);
	return zero_edges.second;
	/*sort(zero_edges.second,[](auto a,auto b){
		return a.first<b.first;
	});*/
}

constexpr auto nfa_to_dfa(auto a,auto b)
{
	
}

int main()
{
	//constexpr auto r=_or(_ch('a'),str('bqwe'));
	constexpr auto w=[](){return _or(_ch('a'),_str('zxcb'));};
	constexpr auto q= run(w);
	for(int i=0;i<q.size();i++)
		printf("%d,%d\n",q[i].first,q[i].second);
	//std::array<int,q> a;
	//constexpr auto q=qqq();
	//std::array<int,q> b;
	
	//nfa_type nfa;
	//std::array<int,run(connect_node(char_node('q'),zero_many_node(string_node('qwer')),string_node('zxcv')),nfa)> a;
	//std::array<int,run(char_node('z'),nfa)> a;
	//auto p=or_node(char_node('a'),char_node('b'));
	//printf("%d,%d\n",test,word_id);
	//for(auto b:nfa)
	//{
		;//printf("%d,%d,%d\n",std::get<0>(b),std::get<1>(b),std::get<2>(b));
	//}
}