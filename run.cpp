#include "run.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
// a+=b


#define convert(type2, type1)                                                  \
    *(type1*)(top + o.destiation_address) = *(type2*)(top + o.source_address);

#define a_b_c(type, op)                                                        \
    *(type*)(top + o.a) = *(type*)(top + o.b) op * (type*)(top + o.c);

#define a_b(type, op) *(type*)(top + o.a) = op * (type*)(top + o.b);

#define self(type, op) op*(type*)(top + o.a);

#define assign(type, op) *(type*)(top + o.a) = *(type*)(top + o.b);

#define load(type, is_load)                                                    \
    if (is_load)                                                               \
        *(type*)(top + o.pos) = **(type**)(top + o.ptr);                       \
    else                                                                       \
        **(type**)(top + o.ptr) = *(type*)(top + o.pos);

#define goto_(type, op)                                                        \
    if (o.have_cond && *(type*)(top + o.goto_flag))                            \
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

void run(op* data, char* top)
{
    size_t i = 0;
    while (true)
    {
        switch (data[i].o1.op_type)
        {
            case 0:
            {
                op_3 o = data[i++].o1;
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
                        all_type(a_b_c, <) break;
                    case 13:
                        all_type(a_b_c, <=) break;
                    case 14:
                        all_type(a_b_c, ==) break;
                    case 15:
                        all_type(a_b_c, !=) break;
                    case 16:
                        all_type(a_b_c, >) break;
                    case 17:
                        all_type(a_b_c, >=) break;
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
                    *(int64_t*)o.pos = o.ptr;
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

int main() {}
