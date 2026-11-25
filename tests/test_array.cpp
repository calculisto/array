#include <doctest/doctest.h>
#include "../include/calculisto/array/array.hpp"
#include <ranges>
#include <stdexcept>
#include <utility>
    using calculisto::array::array_t;

    template <class A, class... ArgsType>
    concept
brace_initializable = requires (ArgsType... args) { A { args... }; };

    template <class... ArgsType>
    concept
deducible = requires (ArgsType... args) { calculisto::array::array_t { args... }; };

TEST_CASE("construction and assignment")
{
    SUBCASE("aggregate, like std::array")
    {
        static_assert (std::is_aggregate_v <array_t <double, 3>>);
        static_assert (std::is_aggregate_v <array_t <double, 0>>);
    }
    SUBCASE("default initialization")
    {
            [[maybe_unused]]
            array_t <int, 2>
        a;
            array_t <int, 2>
        b {};
        CHECK(b == array_t <int, 2> { 0, 0 });
    }
    SUBCASE("initialization")
    {
            array_t <double, 2>
            constexpr
        a { 1.f, 2.f };
        static_assert (a[0] == 1.0);
        static_assert (a[1] == 2.0);
            array_t <double, 2>
            constexpr
        b = { 1.0, 2.0 };
        static_assert (b == a);
    }
    SUBCASE("constants that fit are not narrowing, as for std::array")
    {
            array_t <double, 3>
            constexpr
        a { 0, 0, 1 };
        static_assert (a[2] == 1.0);
    }
    SUBCASE("narrowing is rejected, as for std::array")
    {
#       define TEST_NARROWING(expected, T, ...)                                       \
        static_assert (brace_initializable <array_t    <T, 1>, __VA_ARGS__> == expected); \
        static_assert (brace_initializable <std::array <T, 1>, __VA_ARGS__> == expected);
        TEST_NARROWING(true,  double, double)
        TEST_NARROWING(true,  double, float)
        TEST_NARROWING(false, double, int)
        TEST_NARROWING(false, int,    double)
        TEST_NARROWING(false, int,    long)
        TEST_NARROWING(false, float,  double)
#       undef TEST_NARROWING
    }
    SUBCASE("CTAD")
    {
            constexpr
            array_t
        a { 1, 2 };
        static_assert (std::is_same_v <decltype (a), const array_t <int, 2>>);
        static_assert (a[0] == 1);
        static_assert (a[1] == 2);
    }
    SUBCASE("CTAD requires elements of the same type, as for std::array")
    {
        static_assert (deducible <int, int>);
        static_assert (deducible <double, double, double>);
        static_assert (not deducible <int, double>);
        static_assert (not deducible <double, float>);
    }
    SUBCASE("copy, N == 1")
    {
            array_t <double, 1>
        a { 1.f };
            array_t <double, 1>
        b ( a );
        CHECK(b[0] == 1.0);
            array_t <double, 1> const
        c { 2.0 };
            array_t <double, 1>
        d ( c );
        CHECK(d[0] == 2.0);
        static_assert (std::is_copy_constructible_v <array_t <double, 1>>);
        static_assert (std::is_constructible_v <array_t <double, 1>, array_t <double, 1>&>);
        static_assert (not std::is_convertible_v <int*, array_t <double, 1>>);
        static_assert (not std::is_constructible_v <array_t <double, 2>, int*, int*>);
    }
    SUBCASE("assignment")
    {
            auto
        a = array_t <double, 2> {};
        a = { 0.1, 0.2 };
        CHECK(a[0] == 0.1);
        CHECK(a[1] == 0.2);
            array_t <double, 2>
        b;
        b = a;
        CHECK(b == a);
    }
    SUBCASE("fill")
    {
            array_t <double, 3>
        a { 1.0, 2.0, 3.0 };
        a.fill (4.0);
        CHECK(a == array_t <double, 3> { 4.0, 4.0, 4.0 });
    }
}

    template <class U, class A>
    concept
castable = requires (A a) { calculisto::array::array_cast <U> (a); };

TEST_CASE("array_cast")
{
    SUBCASE("widening")
    {
            constexpr auto
        a = array_cast <double> (array_t { 1, 2 });
        static_assert (std::is_same_v <decltype (a), const array_t <double, 2>>);
        static_assert (a == array_t { 1.0, 2.0 });
    }
    SUBCASE("narrowing is explicit, as with static_cast")
    {
            auto
        a = array_cast <int> (array_t { 1.5, -2.5 });
        static_assert (std::is_same_v <decltype (a), array_t <int, 2>>);
        CHECK(a == array_t { 1, -2 });
    }
    SUBCASE("N == 0")
    {
        static_assert (array_cast <double> (array_t <int, 0> {}) == array_t <double, 0> {});
    }
    SUBCASE("only between convertible types")
    {
        static_assert (castable <double, array_t <int, 2>>);
        static_assert (not castable <int*, array_t <double, 2>>);
    }
}

TEST_CASE("element access")
{
        array_t <double, 3>
    a { 1.0, 2.0, 3.0 };
        array_t <double, 3> const&
    c = a;
    CHECK(a[0] == 1.0);
    CHECK(a[1] == 2.0);
    CHECK(a[2] == 3.0);
    CHECK(c[2] == 3.0);
    CHECK(a.at (0) == 1.0);
    CHECK(a.at (1) == 2.0);
    CHECK(a.at (2) == 3.0);
    CHECK(c.at (2) == 3.0);
    CHECK_THROWS_AS(a.at (3), std::out_of_range);
    CHECK_THROWS_AS(c.at (3), std::out_of_range);
    CHECK(a.front () == 1.0);
    CHECK(a.back  () == 3.0);
    CHECK(c.front () == 1.0);
    CHECK(c.back  () == 3.0);
    CHECK(a.data  () == &a[0]);
    CHECK(c.data  () == &c[0]);
    static_assert (std::is_same_v <decltype (a[0]),      double&>);
    static_assert (std::is_same_v <decltype (c[0]),      double const&>);
    static_assert (std::is_same_v <decltype (a.at (0)),  double&>);
    static_assert (std::is_same_v <decltype (c.at (0)),  double const&>);
    static_assert (std::is_same_v <decltype (a.front ()), double&>);
    static_assert (std::is_same_v <decltype (c.front ()), double const&>);
    static_assert (std::is_same_v <decltype (a.back  ()), double&>);
    static_assert (std::is_same_v <decltype (c.back  ()), double const&>);
    static_assert (std::is_same_v <decltype (a.data  ()), double*>);
    static_assert (std::is_same_v <decltype (c.data  ()), double const*>);
    a[1] = 5.0;
    a.at (2) = 6.0;
    a.front () = 4.0;
    CHECK(a == array_t <double, 3> { 4.0, 5.0, 6.0 });
    a.back () = 7.0;
    *a.data () = 8.0;
    CHECK(a == array_t <double, 3> { 8.0, 5.0, 7.0 });
}

TEST_CASE("iterators and capacity")
{
    SUBCASE("iterator types")
    {
            using
        a_t = array_t <double, 3>;
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().begin   ()), a_t::iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().end     ()), a_t::iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t const&> ().begin   ()), a_t::const_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t const&> ().end     ()), a_t::const_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().cbegin  ()), a_t::const_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().cend    ()), a_t::const_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().rbegin  ()), a_t::reverse_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().rend    ()), a_t::reverse_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t const&> ().rbegin  ()), a_t::const_reverse_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().crbegin ()), a_t::const_reverse_iterator>);
        static_assert (std::is_same_v <decltype (std::declval <a_t&>       ().crend   ()), a_t::const_reverse_iterator>);
        static_assert (std::is_convertible_v <a_t::iterator, a_t::const_iterator>);
        static_assert (std::ranges::contiguous_range <a_t>);
        static_assert (std::ranges::sized_range <a_t>);
        // Make sure that the CI job testing class-type iterators really does.
#       if defined (_LIBCPP_ABI_USE_WRAP_ITER_IN_STD_ARRAY) || defined (_LIBCPP_ABI_BOUNDED_ITERATORS_IN_STD_ARRAY)
        static_assert (not std::is_pointer_v <a_t::iterator>);
        static_assert (not std::is_pointer_v <a_t::const_iterator>);
#       endif
    }
    SUBCASE("traversal")
    {
            array_t <int, 3>
        a { 1, 2, 3 };
        CHECK(std::distance (std::begin  (a), std::end  (a)) == 3);
        CHECK(std::distance (std::rbegin (a), std::rend (a)) == 3);
        CHECK(*a.begin  () == 1);
        CHECK(*a.rbegin () == 3);
            int
        total = 0;
        for (auto e: a)
        {
            total = total * 10 + e;
        }
        CHECK(total == 123);
        for (auto it = a.crbegin (); it != a.crend (); ++it)
        {
            total = total * 10 + *it;
        }
        CHECK(total == 123321);
    }
    SUBCASE("capacity")
    {
            array_t <int, 3>
        a { 1, 2, 3 };
        static_assert (a.size () == 3);
        static_assert (a.max_size () == 3);
        static_assert (not a.empty ());
    }
    SUBCASE("N == 0")
    {
            array_t <int, 0>
        a;
        static_assert (a.size () == 0);
        static_assert (a.empty ());
        CHECK(a.begin () == a.end ());
        CHECK(a == array_t <int, 0> {});
        CHECK(sum (a) == 0);
        CHECK(hypot (array_t <double, 0> {}) == 0.0);
        CHECK(concatenate (a, array_t <int, 1> { 1 }) == array_t <int, 1> { 1 });
    }
}

TEST_CASE("swap")
{
        array_t <int, 3>
    a { 1, 2, 3 };
        array_t <int, 3>
    b { 4, 5, 6 };
    a.swap (b);
    CHECK(a == array_t <int, 3> { 4, 5, 6 });
    CHECK(b == array_t <int, 3> { 1, 2, 3 });
    swap (a, b);
    CHECK(a == array_t <int, 3> { 1, 2, 3 });
    CHECK(b == array_t <int, 3> { 4, 5, 6 });
    std::swap (a, b);
    CHECK(a == array_t <int, 3> { 4, 5, 6 });
    CHECK(b == array_t <int, 3> { 1, 2, 3 });
    {
            using
        std::swap;
        swap (a, b);
    }
    CHECK(a == array_t <int, 3> { 1, 2, 3 });
    CHECK(b == array_t <int, 3> { 4, 5, 6 });
    static_assert (noexcept (a.swap (b)));
    static_assert (std::is_nothrow_swappable_v <array_t <int, 3>>);
}

TEST_CASE("comparison")
{
        constexpr
        array_t
    a { 1, 2, 3 };
        constexpr
        array_t
    b { 1, 3, 0 };
    static_assert (a == a);
    static_assert (a != b);
    static_assert (a <  b);
    static_assert (a <= b);
    static_assert (b >  a);
    static_assert (b >= a);
    static_assert ((a <=> b) < 0);
    CHECK(a != b);
    CHECK(a <  b);
}

TEST_CASE("unary operators")
{
        constexpr
        array_t
    a { 1, -2, 3 };
    static_assert (-a == array_t { -1, 2, -3 });
    static_assert (+a == a);
        array_t <char, 2>
    c { 'a', 'b' };
    // Integral promotion, as for built-in types.
    static_assert (std::is_same_v <decltype (+c), array_t <int, 2>>);
    CHECK(+c == array_t { int { 'a' }, int { 'b' } });
}

TEST_CASE("computed assignment")
{
    SUBCASE("with a scalar returns a reference")
    {
            using
        a_t = array_t <int, 3>;
#       define TEST_COMPUTED_ASSIGNMENT(op) \
        static_assert (std::is_same_v <decltype (std::declval <a_t&> () op##= 1), a_t&>);
        TEST_COMPUTED_ASSIGNMENT(*)
        TEST_COMPUTED_ASSIGNMENT(/)
        TEST_COMPUTED_ASSIGNMENT(%)
        TEST_COMPUTED_ASSIGNMENT(+)
        TEST_COMPUTED_ASSIGNMENT(-)
        TEST_COMPUTED_ASSIGNMENT(^)
        TEST_COMPUTED_ASSIGNMENT(&)
        TEST_COMPUTED_ASSIGNMENT(|)
        TEST_COMPUTED_ASSIGNMENT(<<)
        TEST_COMPUTED_ASSIGNMENT(>>)
#       undef TEST_COMPUTED_ASSIGNMENT
            array_t <double, 3>
        a { 1.0, 2.0, 3.0 };
        CHECK(&(a += 1.0) == &a);
        (a += 1.0) += 10.0;
        CHECK(a == array_t <double, 3> { 13.0, 14.0, 15.0 });
        (a -= 3.0) *= 2.0;
        CHECK(a == array_t <double, 3> { 20.0, 22.0, 24.0 });
    }
    SUBCASE("with an array returns a reference")
    {
            using
        a_t = array_t <int, 3>;
#       define TEST_COMPUTED_ASSIGNMENT(op) \
        static_assert (std::is_same_v <decltype (std::declval <a_t&> () op##= std::declval <a_t> ()), a_t&>);
        TEST_COMPUTED_ASSIGNMENT(*)
        TEST_COMPUTED_ASSIGNMENT(/)
        TEST_COMPUTED_ASSIGNMENT(%)
        TEST_COMPUTED_ASSIGNMENT(+)
        TEST_COMPUTED_ASSIGNMENT(-)
        TEST_COMPUTED_ASSIGNMENT(^)
        TEST_COMPUTED_ASSIGNMENT(&)
        TEST_COMPUTED_ASSIGNMENT(|)
        TEST_COMPUTED_ASSIGNMENT(<<)
        TEST_COMPUTED_ASSIGNMENT(>>)
#       undef TEST_COMPUTED_ASSIGNMENT
            array_t <double, 3>
        a { 1.0, 2.0, 3.0 };
        CHECK(&(a += a) == &a);
    }
    SUBCASE("arithmetic with a scalar")
    {
            array_t <double, 3>
        a { -1.0, -2.0, -3.0 };
        a *= 2.0;
        CHECK(a == array_t <double, 3> { -2.0, -4.0, -6.0 });
        a /= 2.0;
        CHECK(a == array_t <double, 3> { -1.0, -2.0, -3.0 });
        a += 1.0;
        CHECK(a == array_t <double, 3> {  0.0, -1.0, -2.0 });
        a -= 1.0;
        CHECK(a == array_t <double, 3> { -1.0, -2.0, -3.0 });
    }
    SUBCASE("arithmetic with an array")
    {
            array_t <double, 3> const
        a { 1.0, 2.0, 3.0 };
            array_t <double, 3>
        b { -1.0, -2.0, -3.0 };
        b *= a;
        CHECK(b == array_t <double, 3> { -1.0, -4.0, -9.0 });
        b /= a;
        CHECK(b == array_t <double, 3> { -1.0, -2.0, -3.0 });
        b += a;
        CHECK(b == array_t <double, 3> {  0.0,  0.0,  0.0 });
        b -= a;
        CHECK(b == array_t <double, 3> { -1.0, -2.0, -3.0 });
    }
    SUBCASE("arithmetic with an array of another type")
    {
            array_t <double, 2>
        a { 1.5, 2.5 };
        a += array_t { 1, 2 };
        CHECK(a == array_t <double, 2> { 2.5, 4.5 });
    }
    SUBCASE("integer operators")
    {
            array_t <int, 3>
        a { 12, 7, 5 };
        a %= 5;
        CHECK(a == array_t { 2, 2, 0 });
        a <<= 2;
        CHECK(a == array_t { 8, 8, 0 });
        a >>= 1;
        CHECK(a == array_t { 4, 4, 0 });
        a |= 1;
        CHECK(a == array_t { 5, 5, 1 });
        a &= 3;
        CHECK(a == array_t { 1, 1, 1 });
        a ^= 3;
        CHECK(a == array_t { 2, 2, 2 });
        a %= array_t { 2, 3, 4 };
        CHECK(a == array_t { 0, 2, 2 });
        a <<= array_t { 1, 2, 3 };
        CHECK(a == array_t { 0, 8, 16 });
        a >>= array_t { 1, 1, 2 };
        CHECK(a == array_t { 0, 4, 4 });
        a |= array_t { 1, 1, 1 };
        CHECK(a == array_t { 1, 5, 5 });
        a &= array_t { 1, 4, 1 };
        CHECK(a == array_t { 1, 4, 1 });
        a ^= array_t { 1, 4, 0 };
        CHECK(a == array_t { 0, 0, 1 });
    }
    SUBCASE("constexpr")
    {
        static_assert ([]
        {
                array_t
            a { 1, 2 };
            a += 1;
            a *= array_t { 2, 3 };
            return a;
        }() == array_t { 4, 9 });
    }
}

TEST_CASE("binary operators")
{
    SUBCASE("array and array")
    {
            array_t <double, 3> const
        a { 1.0, 2.0, 3.0 };
            array_t <double, 3> const
        b { -1.0, -2.0, -3.0 };
        CHECK(a + b == array_t <double, 3> {  0.0,  0.0,  0.0 });
        CHECK(a - b == array_t <double, 3> {  2.0,  4.0,  6.0 });
        CHECK(a * b == array_t <double, 3> { -1.0, -4.0, -9.0 });
        CHECK(a / b == array_t <double, 3> { -1.0, -1.0, -1.0 });
    }
    SUBCASE("array and scalar")
    {
            array_t <double, 3> const
        a { 1.0, 2.0, 4.0 };
        CHECK(a + 1.0 == array_t <double, 3> {  2.0,  3.0, 5.0  });
        CHECK(1.0 + a == array_t <double, 3> {  2.0,  3.0, 5.0  });
        CHECK(a - 1.0 == array_t <double, 3> {  0.0,  1.0, 3.0  });
        CHECK(1.0 - a == array_t <double, 3> {  0.0, -1.0, -3.0 });
        CHECK(a * 2.0 == array_t <double, 3> {  2.0,  4.0, 8.0  });
        CHECK(2.0 * a == array_t <double, 3> {  2.0,  4.0, 8.0  });
        CHECK(a / 2.0 == array_t <double, 3> {  0.5,  1.0, 2.0  });
        CHECK(4.0 / a == array_t <double, 3> {  4.0,  2.0, 1.0  });
    }
    SUBCASE("result types")
    {
            using
        i_t = array_t <int, 2>;
            using
        d_t = array_t <double, 2>;
        static_assert (std::is_same_v <decltype (i_t {} + i_t {}), i_t>);
        static_assert (std::is_same_v <decltype (i_t {} + d_t {}), d_t>);
        static_assert (std::is_same_v <decltype (i_t {} * 2.0),    d_t>);
        static_assert (std::is_same_v <decltype (2.0 / i_t {}),    d_t>);
    }
    SUBCASE("constexpr")
    {
            constexpr
            array_t
        a { 1, 2 };
            constexpr
            array_t
        b { 3, 4 };
        static_assert (a + b == array_t { 4,  6 });
        static_assert (a - b == array_t { -2, -2 });
        static_assert (a * b == array_t { 3,  8 });
        static_assert (b / a == array_t { 3,  2 });
        static_assert (a * 2 == array_t { 2,  4 });
        static_assert (2 * a == array_t { 2,  4 });
    }
}

TEST_CASE("math functions")
{
    SUBCASE("unary functions")
    {
            array_t <double, 3> const
        a { 0.1, 0.2, 0.3 };
            array_t <double, 3>
        b;
#       define TEST_UNARY_FUNCTION(f) \
        b = f (a);                    \
        CHECK(b[0] == f (a[0]));      \
        CHECK(b[1] == f (a[1]));      \
        CHECK(b[2] == f (a[2]));
        TEST_UNARY_FUNCTION(cos)
        TEST_UNARY_FUNCTION(acos)
        TEST_UNARY_FUNCTION(cosh)
        TEST_UNARY_FUNCTION(sin)
        TEST_UNARY_FUNCTION(asin)
        TEST_UNARY_FUNCTION(sinh)
        TEST_UNARY_FUNCTION(tan)
        TEST_UNARY_FUNCTION(tanh)
        TEST_UNARY_FUNCTION(atan)
        TEST_UNARY_FUNCTION(exp)
        TEST_UNARY_FUNCTION(log)
        TEST_UNARY_FUNCTION(log10)
        TEST_UNARY_FUNCTION(sqrt)
#       undef TEST_UNARY_FUNCTION
    }
    SUBCASE("unary functions on integers")
    {
            array_t <int, 3> const
        a { 1, 4, 9 };
        static_assert (std::is_same_v <decltype (sqrt (a)), array_t <double, 3>>);
        CHECK(sqrt (a) == array_t <double, 3> { 1.0, 2.0, 3.0 });
    }
    SUBCASE("abs")
    {
        CHECK(abs (array_t { -1.0, 2.0, -3.0 }) == array_t { 1.0, 2.0, 3.0 });
        CHECK(abs (array_t { -1, 2, -3 })       == array_t { 1, 2, 3 });
        static_assert (std::is_same_v <decltype (abs (array_t { -1, 2 })), array_t <int, 2>>);
    }
    SUBCASE("pow")
    {
            array_t <double, 3> const
        a { 0.1, 0.2, 0.3 };
            array_t <double, 3>
        b = pow (a, 2.0);
        CHECK(b[0] == pow (a[0], 2.0));
        CHECK(b[1] == pow (a[1], 2.0));
        CHECK(b[2] == pow (a[2], 2.0));
        b = pow (2.0, a);
        CHECK(b[0] == pow (2.0, a[0]));
        CHECK(b[1] == pow (2.0, a[1]));
        CHECK(b[2] == pow (2.0, a[2]));
        b = pow (a, a);
        CHECK(b[0] == pow (a[0], a[0]));
        CHECK(b[1] == pow (a[1], a[1]));
        CHECK(b[2] == pow (a[2], a[2]));
    }
    SUBCASE("hypot")
    {
        CHECK(hypot (array_t <double, 1> { -3.0 }) == 3.0);
        CHECK(hypot (array_t <double, 2> { 3.0, 4.0 }) == 5.0);
        CHECK(hypot (array_t <double, 3> { 2.0, 3.0, 6.0 }) == 7.0);
        CHECK(hypot (array_t <double, 4> { 1.0, 1.0, 1.0, 1.0 }) == 2.0);
        CHECK(hypot (array_t <double, 5> { 1.0, 2.0, 2.0, 4.0, 0.0 }) == 5.0);
    }
    SUBCASE("sum")
    {
            constexpr auto
        a = array_t { 1, 2, 3 };
        static_assert (sum (a) == 6);
        CHECK(sum (array_t { 0.5, 0.25, 0.25 }) == 1.0);
    }
}

    namespace
user
{
        struct
    x_t
    {
            double
        v;
    };
    constexpr x_t cos (x_t a)        { return { a.v + 1.0 }; }
    constexpr x_t pow (x_t a, int n) { return { a.v * n }; }
} // namespace user

    template <class T>
    concept
qualified_pow_exists = requires (T x) { calculisto::array::pow (x, x); };

TEST_CASE("math functions lookup")
{
    SUBCASE("user types are found by argument-dependent lookup")
    {
            array_t <user::x_t, 2>
        a { user::x_t { 1.0 }, user::x_t { 2.0 } };
            auto
        c = cos (a);
        static_assert (std::is_same_v <decltype (c), array_t <user::x_t, 2>>);
        CHECK(c[0].v == 2.0);
        CHECK(c[1].v == 3.0);
            auto
        p = pow (a, 3);
        static_assert (std::is_same_v <decltype (p), array_t <user::x_t, 2>>);
        CHECK(p[0].v == 3.0);
        CHECK(p[1].v == 6.0);
    }
    SUBCASE("arrays of arrays")
    {
            array_t <array_t <double, 2>, 2>
        a { array_t { 0.0, 1.0 }, array_t { 2.0, 3.0 } };
            auto
        c = cos (a);
        static_assert (std::is_same_v <decltype (c), array_t <array_t <double, 2>, 2>>);
        CHECK(c[1][0] == std::cos (2.0));
            auto
        p = pow (a, 2.0);
        CHECK(p[1][1] == 9.0);
    }
    SUBCASE("the standard functions do not leak into the namespace")
    {
        static_assert (not qualified_pow_exists <double>);
        static_assert (qualified_pow_exists <array_t <double, 2>>);
    }
}

    template <class... ArgsType>
    concept
concatenable = requires (ArgsType... args) { calculisto::array::concatenate (args...); };

TEST_CASE("concatenation")
{
    SUBCASE("values")
    {
            array_t <unsigned, 1> const
        a { 1u };
            array_t <unsigned, 2> const
        b { 2u, 3u };
            array_t <unsigned, 3> const
        c { 4u, 5u, 6u };
        CHECK(concatenate (a) == a);
        CHECK(concatenate (a, b) == array_t <unsigned, 3> { 1u, 2u, 3u });
        CHECK(concatenate (a, b, c) == array_t <unsigned, 6> { 1u, 2u, 3u, 4u, 5u, 6u });
        CHECK(concatenate (c, a, b, a) == array_t <unsigned, 7> { 4u, 5u, 6u, 1u, 2u, 3u, 1u });
        CHECK(concatenate (a, array_t <unsigned, 0> {}, b) == array_t <unsigned, 3> { 1u, 2u, 3u });
    }
    SUBCASE("constexpr, also on temporaries")
    {
            constexpr
            array_t
        d { 1, 2 };
            constexpr
            array_t
        e { 3 };
        static_assert (concatenate (d, e) == array_t { 1, 2, 3 });
        static_assert (concatenate (array_t { 1, 2 }, array_t { 3 }) == array_t { 1, 2, 3 });
        static_assert (concatenate (array_t { 1 }, array_t { 2 }, array_t { 3, 4 }) == array_t { 1, 2, 3, 4 });
    }
    SUBCASE("only arrays with the same element type")
    {
        static_assert (concatenable <array_t <int, 1>, array_t <int, 2>>);
        static_assert (not concatenable <>);
        static_assert (not concatenable <array_t <int, 1>, array_t <double, 1>>);
        static_assert (not concatenable <array_t <int, 1>, array_t <int, 1>, array_t <double, 1>>);
    }
    SUBCASE("the comma operator is not overloaded")
    {
            using
        a_t = array_t <int, 1>;
            using
        b_t = array_t <int, 2>;
        static_assert (std::is_same_v <decltype ((std::declval <a_t&> (), std::declval <b_t&> ())), b_t&>);
    }
}
