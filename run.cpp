#include "run.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
// a+=b
struct op_compute {
  unsigned type : 4;
  unsigned op_type : 4;
  unsigned address_type_a : 1;
  unsigned value_type_a : 1;
  unsigned address_type_b : 1;
  unsigned value_type_b : 1;
  unsigned value_type : 4;
};
// flag=a|b
struct op_logic {
  unsigned type : 4;
  unsigned op_type : 4;
  unsigned address_type_a : 1;
  unsigned value_type_a : 1;
  unsigned address_type_b : 1;
  unsigned value_type_b : 1;
  unsigned value_type : 4;
};
// a=b
struct op_load {
  unsigned type : 4;
  unsigned op_type : 4;
  unsigned address_type_a : 1;
  unsigned value_type_a : 1;
  unsigned address_type_b : 1;
  unsigned value_type_b : 1;
  unsigned value_type : 4;
};
// if goto
struct op_goto {
  unsigned type : 4;
  unsigned op_type : 11;
  unsigned value_type_a : 1;
};
// conv
struct op_conv {
  unsigned type : 4;
  unsigned address_type_a : 1;
  unsigned value_type_a : 1;
  unsigned address_type_b : 1;
  unsigned value_type_b : 1;
  unsigned type_a : 4;
  unsigned type_b : 4;
};

union op {
  op_compute o1;
  op_logic o2;
  op_load o3;
  op_goto o4;
  op_conv o5;
};

union type {
  uint64_t _ui64;  // 0
  int64_t _i64;    // 1
  uint32_t _ui32;  // 2
  int32_t _i32;    // 3
  uint16_t _ui16;  // 4
  int16_t _i16;    // 5
  uint8_t _ui8;    // 6
  int8_t _i8;      // 7
  float _f;        // 8
  double _d;       // 9
  long double _ld; // 10
};

/*
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
*/

void run(op *data, char *top) {
  type a;
  type b;
  bool flag;
  size_t i = 0;
  while (true) {
    switch (data[i].o1.type) {
    case 0: {
      op_compute o = data[i++].o1;
      switch (o.op_type) {
      case 0:
        all_type(a_a_b, +) break;
      case 1:
        all_type(a_a_b, -) break;
      case 2:
        all_type(a_a_b, *) break;
      case 3:
        all_type(a_a_b, /) break;
      case 4:
        int_type(a_a_b, |) break;
      case 5:
        int_type(a_a_b, &) break;
      case 6:
        int_type(a_a_b, ^) break;
      case 7:
        int_type(a_a, ~) break;
      case 8:
        int_type(self_a, ++) break;
      case 9:
        int_type(self_a, --) break;
      default:
        assert(0);
      }
      break;
    }
    case 1: {
      op_logic o = data[i++].o2;
      switch (o.op_type) {
      case 0:
        int_type(f_a_b, &&) break;
      case 1:
        int_type(f_a_b, ||) break;
      case 2:
        all_type(f_a_b, <) break;
      case 3:
        all_type(f_a_b, <=) break;
      case 4:
        all_type(f_a_b, ==) break;
      case 5:
        all_type(f_a_b, !=) break;
      case 6:
        all_type(f_a_b, >) break;
      case 7:
        all_type(f_a_b, >=) break;
      case 8:
        flag = !flag;
        break;
      default:
        assert(0);
      }
    }
    case 2: {
      op_load o = data[i++].o3;
      switch (o.op_type) {
      case 0:
        all_type(assign, a, a) break;
      case 1:
        all_type(assign, a, b) break;
      case 2:
        all_type(assign, b, a) break;
      case 3:
        all_type(assign, b, b) break;
      case 4:
        all_type(assign_to_flag, a) break;
      case 5:
        all_type(assign_to_flag, b) break;
      case 6:
        all_type(assign_from_flag, a) break;
      case 7:
        all_type(assign_from_flag, b) break;
      }
    }
    case 3: {
      op_goto o = data[i++].o4;
      switch (o.op_type) {
      case 0:
        i = o.value_type_a ? a._ui64 : *(uint64_t *)(data + i);
        break;
      case 1:
        i += o.value_type_a ? a._ui64 : *(uint64_t *)(data + i);
        break;
      case 2:
        if (flag) {
          i = o.value_type_a ? a._ui64 : *(uint64_t *)(data + i);
        }
      case 3:
        if (flag) {
          i += o.value_type_a ? a._ui64 : *(uint64_t *)(data + i);
        }
        break;
      }
    }
    case 4: {
      op_conv o = data[i++].o5;
      all_type_(o.type_a, o.type_b, convert, _)
    }
    }
  }
fin:;
}

int main() {}
