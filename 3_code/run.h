#pragma once
#include <stdint.h>
//运算符
struct op_3
{
    uint8_t  op_type;//0
	/*
	a=b op c
		case 0:
			+
		case 1:
			-
		case 2:
			*
		case 3:
			/
		case 4:
			|
		case 5:
			&
		case 6:
			^
	a=~b
		case 7:
			~
			
	op a
		case 8:
			++
		case 9:
			--
	
	a=b op c,a is int32_t
		case 10:
			&&
		case 11:
			||
		case 12:
			<
		case 13:
			<=
		case 14:
			==
		case 15:
			!=
		case 16:
			>
		case 17:
			>=
	a=b
		case 18:
			=
	*/
    uint8_t  op;
    uint8_t  value_type;//0,1,2,3,uint8_t - uint64_t 4,5,6,7 int8_t-int64_t 8,9,10 float-loong double
	uint8_t address_a_type;
	uint8_t address_b_type;
	uint8_t address_c_type;
	uint16_t holder;
};

// 寻址和写入内存，这个比较复杂一般来讲
struct op_ptr
{
    uint8_t  op_type;//1
    uint8_t  value_type;//same as above
    uint8_t  load_type;//0 从内存中加载，1写入内存种，2从指令中加载（直接使用ptr的值，不支持long double因为ptr只有8字节）
	uint8_t address_ptr_type;
	uint8_t address_pos_type;
	
};

// 类型转换
struct op_conv
{
    uint8_t  op_type;//2
	//源类型
    uint8_t  source_type;
	//目的类型
    uint8_t  destiation_type;
	//源地址类型
	uint8_t address_source_type;
	//目的地址类型
	uint8_t address_destiation_type;
};

union op {
    op_3    o1;
    op_goto o2;
    op_ptr  o4;
    op_conv o5;
};
