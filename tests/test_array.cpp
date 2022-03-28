#include <doctest/doctest.h>
#include "../include/isto/array/array.hpp"
    using isto::array::array_t;

TEST_CASE("array.hpp")
{
    SUBCASE("default ctor")
    {
            [[maybe_unused]]
            array_t <int, 2> 
        a;
    };
    SUBCASE("converting ctor")
    {
            array_t <double, 2> 
            constexpr
        a { 1.f, 2.f };
        static_assert (a[0] == 1.0);
        static_assert (a[1] == 2.0);
    };
    SUBCASE("CTAD")
    {
            constexpr
            array_t 
        a { 1, 2 };
        static_assert (std::is_same_v <decltype (a), const array_t <int, 2>>);
        static_assert (a[0] == 1);
        static_assert (a[1] == 2);
    };
    /*
    SUBCASE("converting copy ctor")
    {
            constexpr
            array_t 
        a { 1, 2 };
            constexpr
            array_t <double, 2>
        b { a };
        static_assert (std::is_same_v <decltype (b), const array_t <double, 2>>);
        static_assert (b[0] == 1.0);
        static_assert (b[1] == 2.0);
            
    };
    */
    /*
    SUBCASE("converting assignement")
    {
            constexpr
            array_t 
        a { 1, 2 };
            constexpr
            auto
        b = a;
        static_assert (std::is_same_v <decltype (b), const array_t <int, 2>>);
        static_assert (b[0] == 1.0);
        static_assert (b[1] == 2.0);
            
    }
    */
        using 
    point_t = array_t <double, 3>;
        point_t
    a { 1.0, 2.0, 3.0 };
        point_t
    aa;
    aa = a;
    CHECK(aa[0] == 1.0);
    CHECK(aa[1] == 2.0);
    CHECK(aa[2] == 3.0);
    a.fill (1.0);
    CHECK(a[0] == 1.0);
    CHECK(a[1] == 1.0);
    CHECK(a[2] == 1.0);
        point_t
    b { 1.0, 2.0, 3.0 };
    CHECK(b[0] == 1.0);
    CHECK(b[1] == 2.0);
    CHECK(b[2] == 3.0);
    CHECK(b.at (0) == 1.0);
    CHECK(b.at (1) == 2.0);
    CHECK(b.at (2) == 3.0);
    std::swap (a, b);
    CHECK(b[0] == 1.0);
    CHECK(b[1] == 1.0);
    CHECK(b[2] == 1.0);
    CHECK(a[0] == 1.0);
    CHECK(a[1] == 2.0);
    CHECK(a[2] == 3.0);
    CHECK (std::distance (std::begin (a), std::end (a)) == 3);
    CHECK (std::distance (std::rbegin (a), std::rend (a)) == 3);
    static_assert (a.size () == 3, "");
    static_assert (!a.empty (), "");
    CHECK (a.front () == 1.0);
    CHECK (a.back ()  == 3.0);
    b = a;
    CHECK(b[0] == 1.0);
    CHECK(b[1] == 2.0);
    CHECK(b[2] == 3.0);
    CHECK (b == a);
    b = -a;
    CHECK(b[0] == -1.0);
    CHECK(b[1] == -2.0);
    CHECK(b[2] == -3.0);
    b *= 2.0;
    CHECK(b[0] == -2.0);
    CHECK(b[1] == -4.0);
    CHECK(b[2] == -6.0);
    b /= 2.0;
    CHECK(b[0] == -1.0);
    CHECK(b[1] == -2.0);
    CHECK(b[2] == -3.0);
    b += 1.0;
    CHECK(b[0] == 0.0);
    CHECK(b[1] == -1.0);
    CHECK(b[2] == -2.0);
    b -= 1.0;
    CHECK(b[0] == -1.0);
    CHECK(b[1] == -2.0);
    CHECK(b[2] == -3.0);
    b *= a;
    CHECK(b[0] == -1.0);
    CHECK(b[1] == -4.0);
    CHECK(b[2] == -9.0);
    b /= a;
    CHECK(b[0] == -1.0);
    CHECK(b[1] == -2.0);
    CHECK(b[2] == -3.0);
    b += a;
    CHECK(b[0] == 0.0);
    CHECK(b[1] == 0.0);
    CHECK(b[2] == 0.0);
    b -= a;
    CHECK(b[0] == -1.0);
    CHECK(b[1] == -2.0);
    CHECK(b[2] == -3.0);
        auto
    c = a + b;
    CHECK(c[0] == 0.0);
    CHECK(c[1] == 0.0);
    CHECK(c[2] == 0.0);
    c = a - b;
    CHECK(c[0] == 2.0);
    CHECK(c[1] == 4.0);
    CHECK(c[2] == 6.0);
    c = a * b;
    CHECK(c[0] == -1.0);
    CHECK(c[1] == -4.0);
    CHECK(c[2] == -9.0);
    c = a / b;
    CHECK(c[0] == -1.0);
    CHECK(c[1] == -1.0);
    CHECK(c[2] == -1.0);

    c = a - 1.0;
    CHECK(c[0] == 0.0);
    CHECK(c[1] == 1.0);
    CHECK(c[2] == 2.0);



    CHECK (abs (b) == a);
    a = point_t { 0.1, 0.2, 0.3 };


#   define TEST_UNARY_FUNCTION(f) \
    b = f(a); \
    CHECK(b[0] == f(a[0])); \
    CHECK(b[1] == f(a[1])); \
    CHECK(b[2] == f(a[2])); 

    TEST_UNARY_FUNCTION(cos)
    TEST_UNARY_FUNCTION(acos)
    TEST_UNARY_FUNCTION(cosh)
    TEST_UNARY_FUNCTION(sin)
    TEST_UNARY_FUNCTION(asin)
    TEST_UNARY_FUNCTION(sinh)
    TEST_UNARY_FUNCTION(tan)
    TEST_UNARY_FUNCTION(tanh)
    TEST_UNARY_FUNCTION(atan)
    //TEST_UNARY_FUNCTION(atan2)
    TEST_UNARY_FUNCTION(exp)
    TEST_UNARY_FUNCTION(log)
    TEST_UNARY_FUNCTION(log10)
    TEST_UNARY_FUNCTION(sqrt)
#undef TEST_UNARY_FUNCTION
    b = pow (a, 2.0);
    CHECK(b[0] == pow (a[0], 2.)); 
    CHECK(b[1] == pow (a[1], 2.)); 
    CHECK(b[2] == pow (a[2], 2.)); 

    b = pow (2.0, a);
    CHECK(b[0] == pow (2., a[0])); 
    CHECK(b[1] == pow (2., a[1])); 
    CHECK(b[2] == pow (2., a[2])); 

    // Concatenate
    {
            array_t <unsigned, 1> const
        a { 1u };
            array_t <unsigned, 2> const
        b { 2u, 3u };
        CHECK ((concatenate (a, b) == array_t <unsigned, 3> { 1u, 2u, 3u }));
        CHECK (((a, b) == array_t <unsigned, 3> { 1u, 2u, 3u }));
            array_t <unsigned, 3> const
        c { 4u, 5u, 6u };
            array_t <unsigned, 6> const
        expected { 1u, 2u, 3u, 4u, 5u, 6u };
            auto const
        result = concatenate (a, b, c);
        CHECK (result == expected);
    }
    // sum
    {
            constexpr auto
        a = array_t { 1, 2, 3 };
        static_assert (sum (a) == 6);
    }

}
