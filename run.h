#pragma once
#include <stdint.h>
struct op_3
{
    uint8_t  op_type;
    uint8_t  op;
    uint8_t  value_type;
    uint8_t  value__type;
    uint64_t a;
    uint64_t b;
    uint64_t c;
};

// a=b
struct op_ptr
{
    uint8_t  op_type;
    uint8_t  value_type;
    uint8_t  load_type;
    uint8_t  is_immediate;
    uint64_t ptr;
    uint64_t pos;
};

// conv
struct op_conv
{
    uint8_t  op_type;
    uint8_t  source_type;
    uint8_t  destiation_type;
    uint64_t source_address;
    uint64_t destiation_address;
};

struct op_goto
{
    uint8_t  op_type;
    uint8_t  is_relative;
    uint8_t  value_type;
    uint8_t  have_cond;
    uint64_t goto_flag;
    uint64_t goto_address;
};

union op {
    op_3    o1;
    op_goto o2;
    op_ptr  o4;
    op_conv o5;
};
