#include "run.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
// a+=b

#define get_ptr(n) (data->arg[7-n]<=1?\
	data->arg[7-n]==1?(top+data[1+n].value):((char*)&data[1+n])\
	:data->arg[7-n]==2?(stack+data[1+n].value):((char *)data[1+n].value))

#define convert(type2, type1)                                                  \
    *(type1*)get_ptr(0) = *(type2*)get_ptr(1);

#define a_b_c(type, op)                                                        \
    *(type*)get_ptr(0) = *(type*)get_ptr(1) op * get_ptr(0);
	
#define bool_b_c(type, op)                                                        \
    *(uint8_t*)get_ptr(0) = *(type*)get_ptr(1) op * (type*)get_ptr(2);

#define a_b(type, op) *(type*)get_ptr(0) = op * (type*)get_ptr(1);

#define self(type, op) op*(type*)get_ptr(0);

#define assign(type, op) *(type*)get_ptr(0) = *(type*)get_ptr(1);

#define load(type, is_load)                                                    \
switch (is_load){\
	case 0:\
		*(type*)get_ptr(0) = **(type**)get_ptr(1);\
		break;\
	case 1:\
		**(type**)get_ptr(0) = *(type*)get_ptr(1);\
		break;\
	case 2:\
		*(type*)get_ptr(0) = *(type*)&data[2];\
		break;\
}\

#define goto_(type, op)                                                        \
    if (data->arg[2] && *(type*)(top + o.goto_flag))                            \
    {                                                                          \
        i op*(uint64_t*)o.goto_address;                                        \
    }

#define EXPAND(x) x
#define test(...) puts(#__VA_ARGS__);
#define int_case(fun, prefix, case_id, ...)                                    \
    case case_id + 0:                                                          \
        EXPAND(fun(prefix##nt8_t, __VA_ARGS__))                                \
        break;                                                                 \
    case case_id + 1:                                                          \
        EXPAND(fun(prefix##nt16_t, __VA_ARGS__))                               \
        break;                                                                 \
    case case_id + 2:                                                          \
        EXPAND(fun(prefix##nt32_t, __VA_ARGS__))                               \
        break;                                                                 \
    case case_id + 3:                                                          \
        EXPAND(fun(prefix##nt64_t, __VA_ARGS__))                               \
        break;

#define all_type(fun, ...)                                                     \
    switch (o.value_type)                                                      \
    {                                                                          \
        EXPAND(int_case(fun, ui, 0, __VA_ARGS__))                              \
        EXPAND(int_case(fun, i, 4, __VA_ARGS__))                               \
        case 8:                                                                \
            EXPAND(fun(float, __VA_ARGS__))                                    \
            break;                                                             \
        case 9:                                                                \
            EXPAND(fun(double, __VA_ARGS__))                                   \
            break;                                                             \
        case 10:                                                               \
            EXPAND(fun(long double, __VA_ARGS__))                              \
            break;                                                             \
    }

#define int_case_(fun, prefix, case_id, ...)                                   \
    case case_id + 0:                                                          \
        EXPAND(fun(prefix##nt8_t, __VA_ARGS__))                                \
        break;                                                                 \
    case case_id + 1:                                                          \
        EXPAND(fun(prefix##nt16_t, __VA_ARGS__))                               \
        break;                                                                 \
    case case_id + 2:                                                          \
        EXPAND(fun(prefix##nt32_t, __VA_ARGS__))                               \
        break;                                                                 \
    case case_id + 3:                                                          \
        EXPAND(fun(prefix##nt64_t, __VA_ARGS__))                               \
        break;

#define all_type_(type1, type2, fun)                                           \
    switch (type1)                                                             \
    {                                                                          \
        EXPAND(int_case_(all_type__, ui, 0, type2, fun))                       \
        EXPAND(int_case_(all_type__, i, 4, type2, fun))                        \
        case 8:                                                                \
            EXPAND(all_type__(float, type2, fun))                              \
            break;                                                             \
        case 9:                                                                \
            EXPAND(all_type__(double, type2, fun))                             \
            break;                                                             \
        case 10:                                                               \
            EXPAND(all_type__(long double, type2, fun))                        \
            break;                                                             \
    }

#define all_type__(type1, type2, fun)                                          \
    switch (type2)                                                             \
    {                                                                          \
        EXPAND(int_case(fun, ui, 0, type1))                                    \
        EXPAND(int_case(fun, i, 4, type1))                                     \
        case 8:                                                                \
            EXPAND(fun(float, type1))                                          \
            break;                                                             \
        case 9:                                                                \
            EXPAND(fun(double, type1))                                         \
            break;                                                             \
        case 10:                                                               \
            EXPAND(fun(long double, type1))                                    \
            break;                                                             \
    }

#define int_type(fun, ...)                                                     \
    switch (o.value_type)                                                      \
    {                                                                          \
        EXPAND(int_case(fun, ui, 0, __VA_ARGS__))                              \
        EXPAND(int_case(fun, i, 4, __VA_ARGS__))                               \
    }

void run(union{uint64_t value;uint8_t arg[8]} * data, char* top)
{
    size_t i = 0;
    while (true)
    {
        switch (data->arg[0])
        {
            case 0:
            {
                switch (o.op_type)
                {
                    case 0:
                        all_type(a_b_c, +) break;
                    case 1:
                        all_type(a_b_c, -) break;
                    case 2:
                        all_type(a_b_c, *) break;
                    case 3:
                        all_type(a_b_c, /) break;
                    case 4:
                        int_type(a_b_c, |) break;
                    case 5:
                        int_type(a_b_c, &) break;
                    case 6:
                        int_type(a_b_c, ^) break;
                    case 7:
                        int_type(a_b, ~) break;
                    case 8:
                        int_type(self, ++) break;
                    case 9:
                        int_type(self, --) break;
                    case 10:
                        int_type(a_b_c, &&) break;
                    case 11:
                        int_type(a_b_c, ||) break;
                    case 12:
                        all_type(bool_b_c, <) break;
                    case 13:
                        all_type(bool_b_c, <=) break;
                    case 14:
                        all_type(bool_b_c, ==) break;
                    case 15:
                        all_type(bool_b_c, !=) break;
                    case 16:
                        all_type(bool_b_c, >) break;
                    case 17:
                        all_type(bool_b_c, >=) break;
                    case 18:
                        all_type(assign, null) break;
                    default:
                        assert(0);
                }
                break;
            }
            case 1:
            {
                op_ptr o = data[i++].o4;
                if (o.is_immediate)
                {
					
                    *(int64_t*)(top+o.pos) = o.ptr;
                }
                else
                {
                    all_type(load, o.load_type)
                }
                break;
            }
            case 2:
            {
                op_conv o = data[i++].o5;
                all_type_(o.destiation_type, o.source_type, convert) break;
            }
            case 3:
            {
                op_goto o = data[i++].o2;
                if (o.is_relative)
                {
                    all_type(goto_, +=)
                }
                else
                {
                    all_type(goto_, =)
                }
            }
        }
    }
fin:;
}

int main() {
	char buff[1024];
	op o[10];
	o[0].o4.op_type = 1;
	o[0].o4.is_immediate = 1;
	o[0].o4.pos = 0;
	o[0].o4.load_type = 3;
	o[0].o4.ptr = 10;
	o[1].o4.op_type = 1;
	o[1].o4.load_type = 3;
	o[1].o4.is_immediate = 1;
	o[1].o4.pos = 8;
	o[1].o4.ptr = 10;
	o[2].o1.op_type = 0;
	o[2].o1.op = 0;
	o[2].o1.value_type = 3;
	o[2].o1.a = 16;
	o[2].o1.b = 0;
	o[2].o1.c = 8;
	run(o, buff);
}
