# cpp_arithmetic_tuple

Recursively make std::tuple arithmetic with math operators + - * and /.

Example usage:
```cpp
#include "arithmetic_tuple/arithmetic_tuple.h"

struct Foo
{
    float a;
    int b;
    
    using tuple_type = std::tuple<float, int>;
    tuple_type as_tuple() const { return {a, b}; }
    operator tuple_type() const { return {a, b}; }
};

namespace arithmetic_tuple {
    template <> struct Arithmetic<::Foo> { using type = ArithmeticTuple<::Foo::tuple_type>; };
}

using namespace arithmetic_tuple;

int main ()
{
    auto t0 = make_arithmetic(std::make_tuple(1,2.0f)) * 2 - std::make_tuple(1.0f,2);
    std::cout << std::get<0>(t0) << " " << std::get<1>(t0) << std::endl;

    ArithmeticTuple<std::tuple<int,double>> t1 = std::make_tuple(1,2);
    ArithmeticTuple<std::tuple<float,int>> t2 = std::make_tuple(3,4);
    auto t3 = t1 + t1;
    auto t4 = t2 + t2;
    auto t5 = t1 + t2 + 10;
    auto t6 = -10 + t5;
    std::cout << std::get<0>(t1) << " " << std::get<1>(t1) << std::endl;
    std::cout << std::get<0>(t2) << " " << std::get<1>(t2) << std::endl;
    std::cout << std::get<0>(t3) << " " << std::get<1>(t2) << std::endl;
    std::cout << std::get<0>(t4) << " " << std::get<1>(t3) << std::endl;
    std::cout << std::get<0>(t5) << " " << std::get<1>(t5) << std::endl;
    std::cout << std::get<0>(t6) << " " << std::get<1>(t6) << std::endl;
    
    ArithmeticTuple<std::tuple<float,std::tuple<int,double>>> t7 = std::make_tuple(5,std::make_tuple(6,7));
    ArithmeticTuple<std::tuple<float,std::tuple<float,double>>> t8 = std::make_tuple(8,std::make_tuple(9.5,10));
    
    std::cout << std::get<0>(t7) << " " << std::get<0>(std::get<1>(t7)) << " " << std::get<1>(std::get<1>(t7)) << std::endl;
    std::cout << std::get<0>(t8) << " " << std::get<0>(std::get<1>(t8)) << " " << std::get<1>(std::get<1>(t8)) << std::endl;
    
    auto t9 = t7+t8;
    std::cout << std::get<0>(t9) << " " << std::get<0>(std::get<1>(t9)) << " " << std::get<1>(std::get<1>(t9)) << std::endl;
    t2 = t1;
    std::cout << std::get<0>(t1) << " " << std::get<1>(t1) << std::endl;
    std::cout << std::get<0>(t2) << " " << std::get<1>(t2) << std::endl;
    std::tuple<int,double> tpl1 = t1;
    std::cout << std::get<0>(tpl1) << " " << std::get<1>(tpl1) << std::endl;
    
    Foo foo{20,21};
    std::cout << std::get<0>(foo.as_tuple()) << " " << std::get<1>(foo.as_tuple()) << std::endl;
    
    Arithmetic_t<Foo> foo_(foo);
    foo_ = foo_*foo_;
    std::cout << std::get<0>(foo_) << " " << std::get<1>(foo_) << std::endl;
    foo_ -= foo_ * 0.5;
    Foo bar = foo_;
    std::cout << std::get<0>(bar.as_tuple()) << " " << std::get<1>(bar.as_tuple()) << std::endl;
    Foo bar_ = make_arithmetic(bar) * make_arithmetic(0.3) + (1-0.3) * make_arithmetic(foo);
    std::cout << bar_.a << " " << bar_.b << std::endl;
}

```

Output:
```
1 2
1 2
3 4
2 4
6 4
14 16
4 6
5 6 7
8 9.5 10
13 15 17
1 2
1 2
1 2
20 21
400 441
200 221
74 80
```