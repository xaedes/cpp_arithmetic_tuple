#pragma once
// MIT License
// 
// Copyright (c) 2021 xaedes
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <iostream>
#include <tuple>
#include <type_traits>

namespace arithmetic_tuple {

// inspired by https://stackoverflow.com/a/47209931/798588

// https://gist.github.com/ntessore/dc17769676fb3c6daa1f
namespace std14
{
    template<typename T, T... Ints>
    struct integer_sequence
    {
        typedef T value_type;
        static constexpr std::size_t size() { return sizeof...(Ints); }
    };
    
    template<std::size_t... Ints>
    using index_sequence = integer_sequence<std::size_t, Ints...>;
    
    template<typename T, std::size_t N, T... Is>
    struct make_integer_sequence : make_integer_sequence<T, N-1, N-1, Is...> {};
    
    template<typename T, T... Is>
    struct make_integer_sequence<T, 0, Is...> : integer_sequence<T, Is...> {};
    
    template<std::size_t N>
    using make_index_sequence = make_integer_sequence<std::size_t, N>;
    
    template<typename... T>
    using index_sequence_for = make_index_sequence<sizeof...(T)>;
}

//https://stackoverflow.com/a/31763111/798588
template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};
template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <class T> struct Arithmetic { using type = T; };
template <class T> using Arithmetic_t = typename Arithmetic<T>::type;

template <class T> Arithmetic_t<T> make_arithmetic(const T& t) { return Arithmetic_t<T>(t); }


template <class T, class... Args, std::size_t ... Is>
T construct_from_tuple(
    const std::tuple<Args...>& tpl,
    std14::index_sequence<Is...> const &
)
{ return T{ std::get<Is>(tpl)... }; }


template<class Tuple>
struct ArithmeticTuple : public Tuple
{
    using Tuple::Tuple;
    ArithmeticTuple(const Tuple& tpl): Tuple(tpl) {}

    Tuple& as_tuple() { return static_cast<Tuple&>(*this); }
    const Tuple& as_tuple() const { return static_cast<const Tuple&>(*this); }

    template<class T>
    operator T() const 
    {
        return construct_from_tuple<T>(static_cast<const Tuple&>(*this), std14::make_index_sequence<std::tuple_size<Tuple>::value>{});
    }
    
#define ARITHMETIC_TUPLE_OPERATOR( NAME, OP ) \
    template <                                                                                                        \
        class Other                                                                                                   \
    >                                                                                                                 \
    ArithmeticTuple& operator OP##=(                                                                                  \
        const Other&& rhs                                                                                             \
    )                                                                                                                 \
    {                                                                                                                 \
        *this = *this OP rhs;                                                                                         \
        return *this;                                                                                                 \
    }                                                                                                                 \
                                                                                                                      \
    template <                                                                                                        \
        class OtherTuple,                                                                                             \
        std::size_t ... Is,                                                                                           \
        std::enable_if_t<std::tuple_size<Tuple>::value == std::tuple_size<OtherTuple>::value, bool> = true            \
    >                                                                                                                 \
    static ArithmeticTuple NAME(                                                                                      \
        const ArithmeticTuple& lhs,                                                                                   \
        const ArithmeticTuple<OtherTuple>& rhs,                                                                       \
        std14::index_sequence<Is...> const &                                                                          \
    )                                                                                                                 \
    {                                                                                                                 \
        return {                                                                                                      \
            (                                                                                                         \
                Arithmetic_t<typename std::tuple_element<Is, Tuple>::type>(std::get<Is>(lhs))                         \
                OP                                                                                                    \
                Arithmetic_t<typename std::tuple_element<Is, OtherTuple>::type>(std::get<Is>(rhs))                    \
            )...                                                                                                      \
                                                                                                                      \
        };                                                                                                            \
    }                                                                                                                 \
                                                                                                                      \
    template <                                                                                                        \
        class T,                                                                                                      \
        std::size_t ... Is,                                                                                           \
        std::enable_if_t<is_specialization<T, std::tuple>::value, bool> = true                                        \
    >                                                                                                                 \
    static ArithmeticTuple NAME(                                                                                      \
        const ArithmeticTuple& lhs,                                                                                   \
        const T& rhs,                                                                                                 \
        std14::index_sequence<Is...> const &                                                                          \
    )                                                                                                                 \
    {                                                                                                                 \
        return {                                                                                                      \
            (                                                                                                         \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, Tuple>::type>>(std::get<Is>(lhs))                \
            OP                                                                                                        \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, T>::type>>(std::get<Is>(rhs))                    \
            )...                                                                                                      \
        };                                                                                                            \
    }                                                                                                                 \
                                                                                                                      \
    template <                                                                                                        \
        class T,                                                                                                      \
        std::size_t ... Is,                                                                                           \
        std::enable_if_t<is_specialization<T, std::tuple>::value, bool> = true                                        \
    >                                                                                                                 \
    static ArithmeticTuple NAME(                                                                                      \
        const T& lhs,                                                                                                 \
        const ArithmeticTuple& rhs,                                                                                   \
        std14::index_sequence<Is...> const &                                                                          \
    )                                                                                                                 \
    {                                                                                                                 \
        return {                                                                                                      \
            (                                                                                                         \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, T>::type>>(std::get<Is>(lhs))                    \
            OP                                                                                                        \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, Tuple>::type>>(std::get<Is>(rhs))                \
            )...                                                                                                      \
        };                                                                                                            \
    }                                                                                                                 \
                                                                                                                      \
    template <                                                                                                        \
        class T,                                                                                                      \
        std::size_t ... Is,                                                                                           \
        std::enable_if_t<!is_specialization<T, std::tuple>::value, bool> = true,                                      \
        std::enable_if_t<!is_specialization<T, ArithmeticTuple>::value, bool> = true                                  \
    >                                                                                                                 \
    static ArithmeticTuple NAME(                                                                                      \
        const ArithmeticTuple& lhs,                                                                                   \
        const T& rhs,                                                                                                 \
        std14::index_sequence<Is...> const &                                                                          \
    )                                                                                                                 \
    {                                                                                                                 \
        return {                                                                                                      \
            (                                                                                                         \
                static_cast<Arithmetic_t<typename std::tuple_element<Is, Tuple>::type>>(std::get<Is>(lhs))            \
                OP                                                                                                    \
                static_cast<Arithmetic_t<T>>(rhs)                                                                     \
            )...                                                                                                      \
        };                                                                                                            \
    }                                                                                                                 \
    template <                                                                                                        \
        class T,                                                                                                      \
        std::size_t ... Is,                                                                                           \
        std::enable_if_t<!is_specialization<T, std::tuple>::value, bool> = true,                                      \
        std::enable_if_t<!is_specialization<T, ArithmeticTuple>::value, bool> = true                                  \
    >                                                                                                                 \
    static ArithmeticTuple NAME(                                                                                      \
        const T& lhs,                                                                                                 \
        const ArithmeticTuple& rhs,                                                                                   \
        std14::index_sequence<Is...> const &                                                                          \
    )                                                                                                                 \
    {                                                                                                                 \
        return {                                                                                                      \
            (                                                                                                         \
                static_cast<Arithmetic_t<T>>(lhs)                                                                     \
                OP                                                                                                    \
                static_cast<Arithmetic_t<typename std::tuple_element<Is, Tuple>::type>>(std::get<Is>(rhs))            \
            )...                                                                                                      \
        };                                                                                                            \
    }                                                                                                                 
    
    ARITHMETIC_TUPLE_OPERATOR( add, + )
    ARITHMETIC_TUPLE_OPERATOR( sub, - )
    ARITHMETIC_TUPLE_OPERATOR( mul, * )
    ARITHMETIC_TUPLE_OPERATOR( div, / )

#undef ARITHMETIC_TUPLE_OPERATOR

};

template <class...Args> struct Arithmetic<std::tuple<Args...>> { using type = ArithmeticTuple<std::tuple<Args...>>; };

#define ARITHMETIC_TUPLE_OPERATOR( NAME, OP ) \
template <                                                                                                      \
    class Tuple,                                                                                                \
    class Rhs                                                                                                   \
>                                                                                                               \
ArithmeticTuple<Tuple> operator OP (                                                                            \
    const ArithmeticTuple<Tuple> & lhs,                                                                         \
    const Rhs & rhs                                                                                             \
)                                                                                                               \
{                                                                                                               \
    return ArithmeticTuple<Tuple>::NAME(lhs, rhs, std14::make_index_sequence<std::tuple_size<Tuple>::value>{}); \
}                                                                                                               \
                                                                                                                \
template <                                                                                                      \
    class Tuple,                                                                                                \
    class Lhs,                                                                                                  \
    std::enable_if_t<is_specialization<Lhs, ArithmeticTuple>::value == false, bool> = true                      \
>                                                                                                               \
ArithmeticTuple<Tuple> operator OP (                                                                            \
    const Lhs & lhs,                                                                                            \
    const ArithmeticTuple<Tuple> & rhs                                                                          \
)                                                                                                               \
{                                                                                                               \
    return ArithmeticTuple<Tuple>::NAME(lhs, rhs, std14::make_index_sequence<std::tuple_size<Tuple>::value>{}); \
    return rhs OP lhs;                                                                                          \
}                                                                                                               

ARITHMETIC_TUPLE_OPERATOR( add, + )
ARITHMETIC_TUPLE_OPERATOR( sub, - )
ARITHMETIC_TUPLE_OPERATOR( mul, * )
ARITHMETIC_TUPLE_OPERATOR( div, / )

#undef ARITHMETIC_TUPLE_OPERATOR

} // namespace arithmetic_tuple

// Example usage:
// #include "arithmetic_tuple/arithmetic_tuple.h"
// 
// struct Foo
// {
//     float a;
//     int b;
//     
//     using tuple_type = std::tuple<float, int>;
//     tuple_type as_tuple() const { return {a, b}; }
//     operator tuple_type() const { return {a, b}; }
// };
// 
// namespace arithmetic_tuple {
//     template <> struct Arithmetic<::Foo> { using type = ArithmeticTuple<::Foo::tuple_type>; };
// }
// 
// using namespace arithmetic_tuple;
// 
// int main ()
// {
//     auto t0 = make_arithmetic(std::make_tuple(1,2.0f)) * 2 - std::make_tuple(1.0f,2);
//     std::cout << std::get<0>(t0) << " " << std::get<1>(t0) << std::endl;
// 
//     ArithmeticTuple<std::tuple<int,double>> t1 = std::make_tuple(1,2);
//     ArithmeticTuple<std::tuple<float,int>> t2 = std::make_tuple(3,4);
//     auto t3 = t1 + t1;
//     auto t4 = t2 + t2;
//     auto t5 = t1 + t2 + 10;
//     auto t6 = -10 + t5;
//     std::cout << std::get<0>(t1) << " " << std::get<1>(t1) << std::endl;
//     std::cout << std::get<0>(t2) << " " << std::get<1>(t2) << std::endl;
//     std::cout << std::get<0>(t3) << " " << std::get<1>(t2) << std::endl;
//     std::cout << std::get<0>(t4) << " " << std::get<1>(t3) << std::endl;
//     std::cout << std::get<0>(t5) << " " << std::get<1>(t5) << std::endl;
//     std::cout << std::get<0>(t6) << " " << std::get<1>(t6) << std::endl;
//     
//     ArithmeticTuple<std::tuple<float,std::tuple<int,double>>> t7 = std::make_tuple(5,std::make_tuple(6,7));
//     ArithmeticTuple<std::tuple<float,std::tuple<float,double>>> t8 = std::make_tuple(8,std::make_tuple(9.5,10));
//     
//     std::cout << std::get<0>(t7) << " " << std::get<0>(std::get<1>(t7)) << " " << std::get<1>(std::get<1>(t7)) << std::endl;
//     std::cout << std::get<0>(t8) << " " << std::get<0>(std::get<1>(t8)) << " " << std::get<1>(std::get<1>(t8)) << std::endl;
//     
//     auto t9 = t7+t8;
//     std::cout << std::get<0>(t9) << " " << std::get<0>(std::get<1>(t9)) << " " << std::get<1>(std::get<1>(t9)) << std::endl;
//     t2 = t1;
//     std::cout << std::get<0>(t1) << " " << std::get<1>(t1) << std::endl;
//     std::cout << std::get<0>(t2) << " " << std::get<1>(t2) << std::endl;
//     std::tuple<int,double> tpl1 = t1;
//     std::cout << std::get<0>(tpl1) << " " << std::get<1>(tpl1) << std::endl;
//     
//     Foo foo{20,21};
//     std::cout << std::get<0>(foo.as_tuple()) << " " << std::get<1>(foo.as_tuple()) << std::endl;
//     
//     Arithmetic_t<Foo> foo_(foo);
//     foo_ = foo_*foo_;
//     std::cout << std::get<0>(foo_) << " " << std::get<1>(foo_) << std::endl;
//     foo_ -= foo_ * 0.5;
//     Foo bar = foo_;
//     std::cout << std::get<0>(bar.as_tuple()) << " " << std::get<1>(bar.as_tuple()) << std::endl;
//     Foo bar_ = make_arithmetic(bar) * make_arithmetic(0.3) + (1-0.3) * make_arithmetic(foo);
//     std::cout << bar_.a << " " << bar_.b << std::endl;
// }
// /*
// Output:
// 1 2
// 1 2
// 3 4
// 2 4
// 6 4
// 14 16
// 4 6
// 5 6 7
// 8 9.5 10
// 13 15 17
// 1 2
// 1 2
// 1 2
// 20 21
// 400 441
// 200 221
// 74 80
// */