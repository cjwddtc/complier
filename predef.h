#ifndef PREDEF_H_INCLUDED
#define PREDEF_H_INCLUDED
<<<<<<< HEAD

template <class T>
constexpr T get_range_low()
{
    assert(1);
}

template <class T>
constexpr T get_range_hight()
{
    assert(1);
}

template <>
constexpr char get_range_low<char>()
{
    return CHAR_MIN;
}
template <>
constexpr char get_range_hight<char>()
{
    return CHAR_MAX;
}

template <class T>
T get_null_char()
{
    assert(0);
    return T();
}

template <>
char get_null_char<char>()
{
    return 0;
}
=======
#define line_len 128

>>>>>>> github/master

#endif // PREDEF_H_INCLUDED
