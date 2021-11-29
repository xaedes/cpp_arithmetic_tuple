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

template<class Collection, class ValueType>
struct ArithmeticCollection : public Collection
{
    using Collection::Collection;
    ArithmeticCollection(const Collection& tpl): Collection(tpl) {}

    Collection& as_collection() { return static_cast<Collection&>(*this); }
    const Collection& as_collection() const { return static_cast<const Collection&>(*this); }

    template<class OtherCollection>
    operator OtherCollection() const 
    {
        return OtherCollection(begin(), end());
    }
    
#define ARITHMETIC_COLLECTION_OPERATOR( NAME, OP ) \
    template <                                                                                                             \
        class Other                                                                                                        \
    >                                                                                                                      \
    ArithmeticCollection& operator OP##=(                                                                                       \
        const Other&& rhs                                                                                                  \
    )                                                                                                                      \
    {                                                                                                                      \
        for(int i=0; i<size(); ++i) \
            *this[i] = make_arithmetic(*this[i]) OP rhs[i]; \
        return *this;                                                                                                      \
    }                                                                                                                      \
                                                                                                                           \
    template <                                                                                                             \
        class OtherCollection,                                                                                             \
        class OtherValueType                    \
    >                                                                                                                      \
    static ArithmeticCollection NAME(                                                                                           \
        const ArithmeticCollection& lhs,                                                                                        \
        const ArithmeticCollection<OtherCollection, OtherValueType>& rhs                                                                       \
    )                                                                                                                      \
    {                                                                                                                      \
        ArithmeticCollection result(lhs);
        result += rhs;
        return result;

        return {                                                                                                           \
            (                                                                                                              \
                Arithmetic_t<ValueType>(lhs[i])                         \
                OP                                                                                                         \
                Arithmetic_t<OtherValueType>(rhs[i])                    \
            )...                                                                                                           \
                                                                                                                           \
        };                                                                                                                 \
    }                                                                                                                      \
                                                                                                                           \
    template <                                                                                                             \
        class T,                                                                                                           \
        std::size_t ... Is,                                                                                                \
        std::enable_if_t<is_specialization<T, std::tuple>::value, bool> = true                                             \
    >                                                                                                                      \
    static ArithmeticCollection NAME(                                                                                           \
        const ArithmeticCollection& lhs,                                                                                        \
        const T& rhs,                                                                                                      \
        std14::index_sequence<Is...> const &                                                                               \
    )                                                                                                                      \
    {                                                                                                                      \
        return {                                                                                                           \
            (                                                                                                              \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, Collection>::type>>(lhs[i])                \
            OP                                                                                                             \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, T>::type>>(rhs[i])                         \
            )...                                                                                                           \
        };                                                                                                                 \
    }                                                                                                                      \
                                                                                                                           \
    template <                                                                                                             \
        class T,                                                                                                           \
        std::size_t ... Is,                                                                                                \
        std::enable_if_t<is_specialization<T, std::tuple>::value, bool> = true                                             \
    >                                                                                                                      \
    static ArithmeticCollection NAME(                                                                                           \
        const T& lhs,                                                                                                      \
        const ArithmeticCollection& rhs,                                                                                        \
        std14::index_sequence<Is...> const &                                                                               \
    )                                                                                                                      \
    {                                                                                                                      \
        return {                                                                                                           \
            (                                                                                                              \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, T>::type>>(lhs[i])                         \
            OP                                                                                                             \
            static_cast<Arithmetic_t<typename std::tuple_element<Is, Collection>::type>>(rhs[i])                \
            )...                                                                                                           \
        };                                                                                                                 \
    }                                                                                                                      \
                                                                                                                           \
    template <                                                                                                             \
        class T,                                                                                                           \
        std::size_t ... Is,                                                                                                \
        std::enable_if_t<!is_specialization<T, std::tuple>::value, bool> = true,                                           \
        std::enable_if_t<!is_specialization<T, ArithmeticCollection>::value, bool> = true                                       \
    >                                                                                                                      \
    static ArithmeticCollection NAME(                                                                                           \
        const ArithmeticCollection& lhs,                                                                                        \
        const T& rhs,                                                                                                      \
        std14::index_sequence<Is...> const &                                                                               \
    )                                                                                                                      \
    {                                                                                                                      \
        return {                                                                                                           \
            (                                                                                                              \
                static_cast<Arithmetic_t<typename std::tuple_element<Is, Collection>::type>>(lhs[i])            \
                OP                                                                                                         \
                static_cast<Arithmetic_t<T>>(rhs)                                                                          \
            )...                                                                                                           \
        };                                                                                                                 \
    }                                                                                                                      \
    template <                                                                                                             \
        class T,                                                                                                           \
        std::size_t ... Is,                                                                                                \
        std::enable_if_t<!is_specialization<T, std::tuple>::value, bool> = true,                                           \
        std::enable_if_t<!is_specialization<T, ArithmeticCollection>::value, bool> = true                                       \
    >                                                                                                                      \
    static ArithmeticCollection NAME(                                                                                           \
        const T& lhs,                                                                                                      \
        const ArithmeticCollection& rhs,                                                                                        \
        std14::index_sequence<Is...> const &                                                                               \
    )                                                                                                                      \
    {                                                                                                                      \
        return {                                                                                                           \
            (                                                                                                              \
                static_cast<Arithmetic_t<T>>(lhs)                                                                          \
                OP                                                                                                         \
                static_cast<Arithmetic_t<typename std::tuple_element<Is, Collection>::type>>(rhs[i])            \
            )...                                                                                                           \
        };                                                                                                                 \
    }                                                                                                                 
    
    ARITHMETIC_COLLECTION_OPERATOR( add, + )
    ARITHMETIC_COLLECTION_OPERATOR( sub, - )
    ARITHMETIC_COLLECTION_OPERATOR( mul, * )
    ARITHMETIC_COLLECTION_OPERATOR( div, / )

#undef ARITHMETIC_COLLECTION_OPERATOR

};

template <class...Args> struct Arithmetic<std::vector<Args...>> { using type = ArithmeticCollection<std::vector<Args...>>; };
template <class T, int Size> struct Arithmetic<std::array<T, Size>> { using type = ArithmeticCollection<std::array<T, Size>>; };

#define ARITHMETIC_COLLECTION_OPERATOR( NAME, OP ) \
template <                                                                                                                \
    class Collection,                                                                                                     \
    class Rhs                                                                                                             \
>                                                                                                                         \
ArithmeticCollection<Collection> operator OP (                                                                                 \
    const ArithmeticCollection<Collection> & lhs,                                                                              \
    const Rhs & rhs                                                                                                       \
)                                                                                                                         \
{                                                                                                                         \
    return ArithmeticCollection<Collection>::NAME(lhs, rhs, std14::make_index_sequence<std::tuple_size<Collection>::value>{}); \
}                                                                                                                         \
                                                                                                                          \
template <                                                                                                                \
    class Collection,                                                                                                     \
    class Lhs,                                                                                                            \
    std::enable_if_t<is_specialization<Lhs, ArithmeticCollection>::value == false, bool> = true                                \
>                                                                                                                         \
ArithmeticCollection<Collection> operator OP (                                                                                 \
    const Lhs & lhs,                                                                                                      \
    const ArithmeticCollection<Collection> & rhs                                                                               \
)                                                                                                                         \
{                                                                                                                         \
    return ArithmeticCollection<Collection>::NAME(lhs, rhs, std14::make_index_sequence<std::tuple_size<Collection>::value>{}); \
    return rhs OP lhs;                                                                                                    \
}                                                                                                               

ARITHMETIC_COLLECTION_OPERATOR( add, + )
ARITHMETIC_COLLECTION_OPERATOR( sub, - )
ARITHMETIC_COLLECTION_OPERATOR( mul, * )
ARITHMETIC_COLLECTION_OPERATOR( div, / )

#undef ARITHMETIC_COLLECTION_OPERATOR

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
//     template <> struct Arithmetic<::Foo> { using type = ArithmeticCollection<::Foo::tuple_type>; };
// }
// 
// using namespace arithmetic_tuple;
// 
// int main ()
// {
//     auto t0 = make_arithmetic(std::make_tuple(1,2.0f)) * 2 - std::make_tuple(1.0f,2);
//     std::cout << std::get<0>(t0) << " " << std::get<1>(t0) << std::endl;
// 
//     ArithmeticCollection<std::tuple<int,double>> t1 = std::make_tuple(1,2);
//     ArithmeticCollection<std::tuple<float,int>> t2 = std::make_tuple(3,4);
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
//     ArithmeticCollection<std::tuple<float,std::tuple<int,double>>> t7 = std::make_tuple(5,std::make_tuple(6,7));
//     ArithmeticCollection<std::tuple<float,std::tuple<float,double>>> t8 = std::make_tuple(8,std::make_tuple(9.5,10));
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