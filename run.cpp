#include <stdint.h>
#include <stdio.h>
#include <assert.h>
struct op_all {
	unsigned type : 1;
	unsigned op_type : 4;//+-*/|>>==
	unsigned value_type : 3;//1,2,4,8 float double
};
struct op_int {
	unsigned type : 1;
	unsigned op_type : 5;//+-*/|>>==
	unsigned value_type : 2;//1,2,4,8 float double
};
//a+=b
struct op_compute {
	unsigned type : 4;
	unsigned op_type : 5;
	unsigned a_address_type : 1;
	unsigned b_address_type : 2;
	unsigned value_type : 4;
};
//flag=a|b
struct op_logic {
	unsigned type : 4;
	unsigned op_type : 4;
	unsigned a_address_type : 2;
	unsigned b_address_type : 2;
	unsigned value_type : 4;
};
//a=b
struct op_load{
	unsigned type : 4;
	unsigned src : 3;
	unsigned des : 2;
	unsigned value_type : 4;
	unsigned null : 3;
};
//if goto
struct op_goto {
	unsigned type : 4;
	unsigned op_type : 6;
	unsigned a_address_type : 2;
	unsigned value_type : 4;
};
//conv
struct op_conv {
	unsigned type : 4;
	unsigned a_address_type : 1;
	unsigned b_address_type : 1;
	unsigned src : 4;
	unsigned des : 4;
	unsigned null : 2;
};

union op {
	op_compute o1;
	op_logic o2;
	op_load o3;
	op_goto o4;
	op_conv o5;
};

union type {
	uint64_t u64;//0
	int64_t i64;//1
	uint32_t u32;//2
	int32_t i32;//3
	uint16_t u16;//4
	int16_t i16;//5
	uint8_t u8;//6
	int8_t i8;//7
	float f;//8
	double d;//9
	long double ld;//10
};

#define get_value(var,type) type<5?\
								type<3?\
									type==0?\
										var.u64:\
										type==1?var.i64:var.u32\
									:type==3?\
										var.i32:\
										var.u16\
								:type<8?\
									type==5?\
										var.i16\
										:type==6?var.u8:var.i8\
									:type==8?\
										var.f\
										:type==9?var.d:var.ld


#define get_address_value(value_type,value) (o.##value##_address_type==0?get_value(value,value_type):*(value_type *)(o.##value##_address_type==1?value:((data+i),i+=(sizeof(value_type)-1)/2+1)))

#define load_src() (o.src==0?flag:o.src==1?*(o.value_type*)&a:o.src==2?o.value)

#define do_op_type(type,op) \
get_address_value(type,a) op= get_address_value(type,b);\

#define do_cp_type(type,op) \
flag=get_rvalue(type,a) op get_rvalue(type,b);\

#define do_one_op_type(type,op) \
get_lvalue(type,a) =op get_rvalue(type,a);\

#define do_self_op_type(type,op) \
op get_lvalue(type,a);

#define load_type(type,ptr,s) \
s = *(type*)(ptr);\

#define EXPAND( x ) x
#define all_type(type,fun,...) \
switch (type) {\
	\
case 0:\
		EXPAND(fun( uint8_t ,__VA_ARGS__))\
		break; \
case 1:\
		EXPAND(fun( uint16_t , __VA_ARGS__))\
		break; \
case 2:\
		EXPAND(fun( uint32_t , __VA_ARGS__))\
		break; \
case 3:\
		EXPAND(fun( uint64_t , __VA_ARGS__))\
		break; \
case 4:\
		EXPAND(fun( float , __VA_ARGS__))\
		break; \
case 5:\
		EXPAND(fun( double , __VA_ARGS__))\
		break; \
}\

#define int_type(type,fun,...) \
switch (type) {\
	\
case 0:\
		EXPAND(fun( uint8_t ,__VA_ARGS__))\
		break; \
case 1:\
		EXPAND(fun( uint16_t , __VA_ARGS__))\
		break; \
case 2:\
		EXPAND(fun( uint32_t , __VA_ARGS__))\
		break; \
case 3:\
		EXPAND(fun( uint64_t , __VA_ARGS__))\
		break; \
}\

#define do_self(op) int_type(o.value_type,do_self_op_type,op)
#define do_one(op) int_type(o.value_type,do_one_op_type,op)
#define do_ip(op) int_type(o.value_type, do_op_type, op );
#define do_op(op) all_type(o.value_type,do_op_type,op);

#define do_cp(op) all_type(o.value_type,do_cp_type,op);

#define load_all_type(s,ptr) all_type(o.value_type,load_type,s,ptr);

#define not_op(type,a) flag=!*(type*)&a;

#define move_flag(type,a) *(type*)&a=flag;

#define switch_conv(s,a1,a2,a3,a4) \
switch (o.value_type)\
{\
	case 0:\
		*(s*)a = *(a1*)a;\
		break;\
	case 1:\
		*(s*)a = *(a2*)a;\
		break;\
	case 2:\
		*(s*)a = *(a3*)a;\
		break;\
	case 3:\
		*(s*)a = *(a4*)a;\
		break;\
		default:\
			break;\
}\

#define switch_all(s,a1,a2,a3,a4,a5,a6) \
case s:\
switch_conv(a1,a2,a3,a4,a5,)\
break;\
case s+1:\
switch_conv(a2,a1,a3,a4,a5,)\
break;\
case s+2:\
switch_conv(a3,a1,a2,a4,a5,)\
break;\
case s+3:\
switch_conv(a4,a1,a2,a3,a5,)\
break;\
case s+4:\
switch_conv(a5,a1,a2,a3,a4)\
break;\
case s+5:\
switch_conv(a6,a1,a2,a3,a4,a5)\
break;\
case s+6:\
switch (o.value_type)\
{\
case 0:\
*(a5*)a = *(a6*)a; \
break;\
case 1:\
*(a6*)a = *(a5*)a; \
break;\
}
#define plus_size(type,i) \
i+=sizeof(type)+1;


void run(op *data, char *top, char *current)
{
	uint64_t a;
	uint64_t b;
	bool flag;
	size_t i = 0;
	while (true) {
		switch (data[i].o1.type) {
		case 0:
		{
			op_compute o = data[i++].o1;
			switch (o.op_type) {
			case 0:
				do_op(+)
					break;
			case 1:
				do_op(-)
					break;
			case 2:
				do_op(*)
					break;
			case 3:
				do_op(/)
					break;
			case 4:
				do_ip(| )
					break;
			case 5:
				do_ip(& )
					break;
			case 6:
				do_ip(^ )
					break;
			case 7:
				do_one(~)
			case 8:
				do_self(++)
			case 9:
				do_self(++)
			default:
				assert(0);
			}
			break;
		}
		case 1:
		{
			op_logic o = data[i++].o2;
			switch (o.op_type)
			{
			case 0:
				do_cp(&&)
					break;
			case 1:
				do_cp(|| )
					break;
			case 2:
				do_cp(<)
					break;
			case 3:
				do_cp(<=)
					break;
			case 4:
				do_cp(>)
					break;
			case 5:
				do_cp(>=)
					break;
			case 6:
				do_cp(==)
					break;
			case 7:
				do_cp(!=)
					break;
			case 8:
				flag = !flag;
					break;
			default:
				assert(0);
			}
		}
		case 2:
			op_load o = data[i++].o3;
		}
		if (data[i].a.type) {
			op_all o = data[i].a;
			switch (o.op_type) {
				//加减乘除
			case 1:
					break;
			case 2:
				do_op(-)
					break;
			case 3:
				do_op(*)
					break;
			case 4:
				do_op(/ )
					break;
				//间接寻址
			case 5:
				load_all_type(a, a)
					break;
			case 6:
				load_all_type(b,a)
					break;
				//比较大小
			case 7:
				do_cp(<)
					break;
			case 8:
				do_cp(<= )
					break;
			case 9:
				do_cp(== )
					break;
				//读取字面值
			case 10:
				load_all_type(data + i + 1, a)
				all_type(o.value_type, plus_size,i)
					break;
			case 11:
				load_all_type(data + i + 1, b)
				all_type(o.value_type, plus_size, i)
					break;
			case 12:
				goto fin;
			}
		}
		else {
			op_int o = data[i].o2;
			switch (o.op_type) {
			case 0:
				switch (o.value_type)
				{
				case 0:
					++(*(uint8_t*)&a);
					break;
				case 1:
					++(*(uint16_t*)&a);
					break;
				case 2:
					++(*(uint32_t*)&a);
					break;
				case 3:
					++(*(uint64_t*)&a);
					break;
				}
				++i;
				break;
			case 1:
				switch (o.value_type)
				{
				case 0:
					--(*(uint8_t*)&a);
					break;
				case 1:
					--(*(uint16_t*)&a);
					break;
				case 2:
					--(*(uint32_t*)&a);
					break;
				case 3:
					--(*(uint64_t*)&a);
					break;
				}
				++i;
				break;
			case 2:
				do_cp(&&)
					break;
			case 3:
				do_cp(|| )
					break;
			case 4:
				int_type(o.value_type, not_op, a)
					break;
			case 5:
				int_type(o.value_type, move_flag, a)
					break;
			case 6:
				int_type(o.value_type, move_flag, b)
					break;
			case 7:
				if(o.value_type&0x4){
					switch (o.value_type) {
					case 0:
						a = b;
						break;
					case 1:
						b = a;
						break;
					case 2: {
						uint64_t t = a;
						a = b;
						b = t;
						break;
					}
					}
				}
				else {
					o.value_type |= 0x3;
					if (o.value_type || flag)
						switch (o.value_type) {
						case 1:
							i += a;
							break;
						case 2:
							i = a;
							break;
						}
					else ++i;
				}
				i++;
				break;
			case 9:
				int_type(o.value_type, do_op_type, | )
					break;
			case 10:
				int_type(o.value_type, do_op_type, &)
					break;
			case 11:
				int_type(o.value_type, do_op_type, ^)
					break; 
			switch_all(12,uint8_t,uint16_t,uint32_t,uint64_t,float,double)
			}
		}
	}
	fin:;
}


int main() {

}