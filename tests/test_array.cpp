#include <doctest/doctest.h>
#include <calculisto/array/array.hpp>
#include <cmath>
#include <complex>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <string>
#include <tuple>
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
        TEST_NARROWING(false, int,    long long)
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
        CHECK(euclidean_norm (array_t <double, 0> {}) == 0.0);
        CHECK(concatenate (a, array_t <int, 1> { 1 }) == array_t <int, 1> { 1 });
    }
}

    template <std::size_t I, class A>
    concept
gettable = requires (A a) { get <I> (a); };

TEST_CASE("tuple protocol, as for std::array")
{
    SUBCASE("tuple_size and tuple_element")
    {
            using
        a_t = array_t <double, 3>;
        static_assert (std::tuple_size_v <a_t> == 3);
        static_assert (std::tuple_size_v <array_t <int, 0>> == 0);
        static_assert (std::is_same_v <std::tuple_element_t <0, a_t>, double>);
        static_assert (std::is_same_v <std::tuple_element_t <2, a_t>, double>);
    }
    SUBCASE("get")
    {
            constexpr
            array_t
        c { 1, 2, 3 };
        static_assert (get <0> (c) == 1);
        static_assert (get <2> (c) == 3);
            array_t
        a { 1, 2, 3 };
        get <1> (a) = 5;
        CHECK(a == array_t { 1, 5, 3 });
        static_assert (std::is_same_v <decltype (get <0> (a)), int&>);
        static_assert (std::is_same_v <decltype (get <0> (std::as_const (a))), int const&>);
        static_assert (std::is_same_v <decltype (get <0> (std::move (a))), int&&>);
        static_assert (std::is_same_v <decltype (get <0> (std::move (std::as_const (a)))), int const&&>);
        static_assert (gettable <2, array_t <int, 3>>);
        static_assert (not gettable <3, array_t <int, 3>>);
    }
    SUBCASE("structured bindings")
    {
            array_t
        a { 1.0, 2.0, 3.0 };
        {
                auto
            [x, y, z] = a;
            CHECK(x == 1.0);
            CHECK(y == 2.0);
            CHECK(z == 3.0);
        }
        {
                auto&
            [x, y, z] = a;
            x = 4.0;
            z = 6.0;
            CHECK(a == array_t { 4.0, 2.0, 6.0 });
            static_assert (std::is_same_v <decltype (y), double>);
        }
        static_assert ([]
        {
                auto
            [x, y] = array_t { 1, 2 };
            return 10 * x + y;
        }() == 12);
            int
        total = 0;
        for (auto [i, j] : array_t { array_t { 1, 2 }, array_t { 3, 4 } })
        {
            total += 10 * i + j;
        }
        CHECK(total == 12 + 34);
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
    SUBCASE("with an element of the array itself")
    {
        // The scalar is a reference to the first element, which the
        // computed assignment modifies first: the other elements must still
        // use its original value.
#       define TEST_SELF_ASSIGNMENT(T, op, init, expected) \
        {                                                  \
                array_t <T, 3>                             \
            a init;                                        \
            a op##= a[0];                                  \
            CHECK(a == array_t <T, 3> expected);           \
        }
        TEST_SELF_ASSIGNMENT(double, +,  ({ 1.0, 2.0, 3.0 }), ({ 2.0, 3.0, 4.0 }))
        TEST_SELF_ASSIGNMENT(double, -,  ({ 1.0, 2.0, 3.0 }), ({ 0.0, 1.0, 2.0 }))
        TEST_SELF_ASSIGNMENT(double, *,  ({ 2.0, 3.0, 4.0 }), ({ 4.0, 6.0, 8.0 }))
        TEST_SELF_ASSIGNMENT(double, /,  ({ 2.0, 4.0, 8.0 }), ({ 1.0, 2.0, 4.0 }))
        TEST_SELF_ASSIGNMENT(int,    %,  ({ 3, 5, 7 }),       ({ 0, 2, 1 }))
        TEST_SELF_ASSIGNMENT(int,    ^,  ({ 1, 2, 3 }),       ({ 0, 3, 2 }))
        TEST_SELF_ASSIGNMENT(int,    &,  ({ 3, 5, 6 }),       ({ 3, 1, 2 }))
        TEST_SELF_ASSIGNMENT(int,    |,  ({ 1, 2, 4 }),       ({ 1, 3, 5 }))
        TEST_SELF_ASSIGNMENT(int,    <<, ({ 1, 2, 3 }),       ({ 2, 4, 6 }))
        TEST_SELF_ASSIGNMENT(int,    >>, ({ 1, 4, 6 }),       ({ 0, 2, 3 }))
#       undef TEST_SELF_ASSIGNMENT
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
    // The results are default-initialized before being assigned: check the
    // cases where this differs from zeroing them.
    SUBCASE("constexpr, N == 0")
    {
            constexpr
            array_t <int, 0>
        e {};
        static_assert (e + e == e);
        static_assert (e * 2 == e);
        static_assert (2 - e == e);
        static_assert (-e == e);
    }
    SUBCASE("elements with a constructor")
    {
            array_t <std::string, 2> const
        a { "a", "b" };
        CHECK(a + std::string { "x" } == array_t <std::string, 2> { "ax", "bx" });
        CHECK(std::string { "x" } + a == array_t <std::string, 2> { "xa", "xb" });
        CHECK(a + a == array_t <std::string, 2> { "aa", "bb" });
    }
}

// A quantity with a unit, like a physical quantity: its default value is
// zero, but it can't be built from a plain number, which has no unit.
    namespace
units
{
        struct
    metre_t {};
        inline constexpr
        metre_t
    metre;

        struct
    length_t
    {
            constexpr
        length_t () = default;
            constexpr
        length_t (double v, metre_t)
            : metres { v }
        {}
            double
        metres = 0.0;
            friend constexpr length_t
        operator + (length_t a, length_t b)
        {
            return { a.metres + b.metres, metre };
        }
            friend constexpr bool
        operator == (length_t, length_t) = default;
    };
} // namespace units

    template <class Y, class X>
    concept
has_atan2 = requires (Y y, X x) { atan2 (y, x); };

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
        // The result type is that of std::abs, as for the other functions:
        // small integers are promoted, as by unary minus...
            array_t <short, 2> const
        s { -1, 2 };
        static_assert (std::is_same_v <decltype (abs (s)), array_t <int, 2>>);
        static_assert (std::is_same_v <decltype (-s), array_t <int, 2>>);
        CHECK(abs (s) == array_t { 1, 2 });
        // ...and the absolute value of a complex number is real.
            array_t <std::complex <double>, 2> const
        z { std::complex <double> { 3.0, 4.0 }, std::complex <double> { 0.0, -2.0 } };
        static_assert (std::is_same_v <decltype (abs (z)), array_t <double, 2>>);
        CHECK(abs (z) == array_t { std::abs (z[0]), std::abs (z[1]) });
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
    SUBCASE("atan2")
    {
        // The four quadrants, where atan2 (y, x) differs from atan (y / x).
            array_t <double, 4> const
        y { 1.0,  1.0, -1.0, -1.0 };
            array_t <double, 4> const
        x { 1.0, -1.0, -1.0,  1.0 };
            array_t <double, 4>
        b = atan2 (y, x);
        for (std::size_t i = 0; i < 4; ++i)
        {
            CHECK(b[i] == std::atan2 (y[i], x[i]));
        }
        b = atan2 (y, -2.0);
        for (std::size_t i = 0; i < 4; ++i)
        {
            CHECK(b[i] == std::atan2 (y[i], -2.0));
        }
        b = atan2 (-2.0, x);
        for (std::size_t i = 0; i < 4; ++i)
        {
            CHECK(b[i] == std::atan2 (-2.0, x[i]));
        }
        static_assert (std::is_same_v <decltype (atan2 (array_t { 1, 2 }, array_t { 3, 4 })), array_t <double, 2>>);
        static_assert (has_atan2 <array_t <double, 2>, array_t <double, 2>>);
        static_assert (has_atan2 <array_t <double, 2>, double>);
        static_assert (has_atan2 <double, array_t <double, 2>>);
        static_assert (not has_atan2 <array_t <std::string, 2>, array_t <std::string, 2>>);
    }
    SUBCASE("euclidean_norm")
    {
        // std::hypot need not be correctly rounded, so chained results are
        // compared with a tolerance of a few ulps.
            auto
        ulps = [](double x){ return doctest::Approx (x).epsilon (8 * std::numeric_limits <double>::epsilon ()); };
        CHECK(euclidean_norm (array_t <double, 0> {}) == 0.0);
        CHECK(euclidean_norm (array_t <double, 1> { -3.0 }) == 3.0);
        CHECK(euclidean_norm (array_t <double, 2> { 3.0, 4.0 }) == ulps (5.0));
        CHECK(euclidean_norm (array_t <double, 3> { 2.0, 3.0, 6.0 }) == ulps (7.0));
        CHECK(euclidean_norm (array_t <double, 4> { 1.0, 1.0, 1.0, 1.0 }) == ulps (2.0));
        CHECK(euclidean_norm (array_t <double, 5> { 1.0, 2.0, 2.0, 4.0, 0.0 }) == ulps (5.0));
    }
    SUBCASE("euclidean_norm does not overflow or underflow in the intermediate squares")
    {
        // hypot (x, 0) is exactly |x|, so these are exact.
        CHECK(euclidean_norm (array_t { 1e200 }) == 1e200);
        CHECK(euclidean_norm (array_t { 1e200, 1e200, 0.0, 0.0 })   == std::hypot (1e200, 1e200));
        CHECK(euclidean_norm (array_t { 3e-200, 4e-200, 0.0, 0.0 }) == std::hypot (3e-200, 4e-200));
        CHECK(euclidean_norm (array_t { 3e30f, 4e30f, 0.f, 0.f })   == std::hypot (3e30f, 4e30f));
        // int values are converted to double, as by std::hypot, instead of
        // overflowing (undefined behavior) when squared.
        static_assert (std::is_same_v <decltype (euclidean_norm (array_t { 1, 2, 3, 4 })), double>);
        CHECK(euclidean_norm (array_t { 30000, 40000, 0, 0 }) == std::hypot (30000, 40000));
    }
    SUBCASE("sum")
    {
            constexpr auto
        a = array_t { 1, 2, 3 };
        static_assert (sum (a) == 6);
        CHECK(sum (array_t { 0.5, 0.25, 0.25 }) == 1.0);
        CHECK(sum (array_t <double, 0> {}) == 0.0);
        // The sum starts from a value-initialized element, not from 0: a
        // quantity with a unit can't be built from a plain number...
            using
        units::length_t;
            using
        units::metre;
        static_assert (not std::is_constructible_v <length_t, int>);
        static_assert (sum (array_t <length_t, 3> {
              length_t { 1.0, metre }
            , length_t { 2.0, metre }
            , length_t { 3.0, metre }
        }) == length_t { 6.0, metre });
        static_assert (sum (array_t <length_t, 0> {}) == length_t {});
        // ...and 0 would construct a std::string from a null pointer.
        CHECK(sum (array_t <std::string, 3> { "a", "b", "c" }) == "abc");
    }
}

// Element-wise comparison with the std:: functions, NaNs being equal.
    template <class X, class Y>
    constexpr bool
same_value (X x, Y y)
{
    if constexpr (std::is_floating_point_v <X> and std::is_floating_point_v <Y>)
    {
        if (std::isnan (x) or std::isnan (y))
        {
            return std::isnan (x) and std::isnan (y);
        }
    }
    return x == y;
}

// Copies an array through volatile reads, so that the compiler can't evaluate
// the functions applied to it at compile time: GCC computes the std:: math
// functions exactly at compile time, while the C library may differ by an ulp
// at run time (e.g. expm1 (1)).
    template <class T, std::size_t N>
    array_t <T, N>
opaque (array_t <T, N> const& a)
{
        array_t <T, N>
    r;
    for (std::size_t i = 0; i < N; ++i)
    {
            T volatile
        v = a[i];
        r[i] = v;
    }
    return r;
}

// f (a) must have the element type of std::f (a[i]) and its values.
#define CHECK_LIKE_STD_UNARY(f, a)                                          \
    {                                                                       \
            auto const                                                      \
        r = f (a);                                                          \
        static_assert (std::is_same_v <                                     \
              typename std::remove_cvref_t <decltype (r)>::value_type       \
            , decltype (std::f (a[0]))                                      \
        >);                                                                 \
        for (std::size_t i = 0; i < a.size (); ++i)                         \
        {                                                                   \
            CHECK(same_value (r[i], std::f (a[i])));                        \
        }                                                                   \
    }

// The same for the three forms of a binary function: two arrays, an array
// and a scalar, a scalar and an array.
#define CHECK_LIKE_STD_BINARY(f, x, y)                                      \
    {                                                                       \
            auto const                                                      \
        r = f (x, y);                                                       \
        static_assert (std::is_same_v <                                     \
              typename std::remove_cvref_t <decltype (r)>::value_type       \
            , decltype (std::f (x[0], y[0]))                                \
        >);                                                                 \
            auto const                                                      \
        rs = f (x, y[1]);                                                   \
            auto const                                                      \
        sr = f (x[1], y);                                                   \
        for (std::size_t i = 0; i < x.size (); ++i)                         \
        {                                                                   \
            CHECK(same_value (r[i],  std::f (x[i], y[i])));                 \
            CHECK(same_value (rs[i], std::f (x[i], y[1])));                 \
            CHECK(same_value (sr[i], std::f (x[1], y[i])));                 \
        }                                                                   \
    }

// The same for the seven forms of a ternary function: each argument is an
// array or a scalar, at least one being an array.
#define CHECK_LIKE_STD_TERNARY(f, x, y, z)                                  \
    {                                                                       \
            auto const                                                      \
        aaa = f (x,    y,    z);                                            \
        static_assert (std::is_same_v <                                     \
              typename std::remove_cvref_t <decltype (aaa)>::value_type     \
            , decltype (std::f (x[0], y[0], z[0]))                          \
        >);                                                                 \
            auto const                                                      \
        aas = f (x,    y,    z[1]);                                         \
            auto const                                                      \
        asa = f (x,    y[1], z);                                            \
            auto const                                                      \
        saa = f (x[1], y,    z);                                            \
            auto const                                                      \
        ass = f (x,    y[1], z[1]);                                         \
            auto const                                                      \
        sas = f (x[1], y,    z[1]);                                         \
            auto const                                                      \
        ssa = f (x[1], y[1], z);                                            \
        for (std::size_t i = 0; i < x.size (); ++i)                         \
        {                                                                   \
            CHECK(same_value (aaa[i], std::f (x[i], y[i], z[i])));          \
            CHECK(same_value (aas[i], std::f (x[i], y[i], z[1])));          \
            CHECK(same_value (asa[i], std::f (x[i], y[1], z[i])));          \
            CHECK(same_value (saa[i], std::f (x[1], y[i], z[i])));          \
            CHECK(same_value (ass[i], std::f (x[i], y[1], z[1])));          \
            CHECK(same_value (sas[i], std::f (x[1], y[i], z[1])));          \
            CHECK(same_value (ssa[i], std::f (x[1], y[1], z[i])));          \
        }                                                                   \
    }

TEST_CASE("math functions behave like their std:: counterparts")
{
        using
    limits = std::numeric_limits <double>;
        auto const
    finite = opaque (array_t <double, 13> { -100.0, -2.5, -1.5, -1.0, -0.5, -0.0, 0.0, 0.25, 0.5, 1.0, 1.5, 2.5, 100.0 });
        auto const
    special = opaque (array_t <double, 6> { limits::infinity (), -limits::infinity (), limits::quiet_NaN (), limits::denorm_min (), limits::max (), -limits::max () });
        auto const
    values = concatenate (finite, special);
        auto const
    integers = opaque (array_t <int, 5> { -3, -1, 0, 1, 3 });

    SUBCASE("unary functions")
    {
#       define CHECK_LIKE_STD(f)                \
        CHECK_LIKE_STD_UNARY(f, values)         \
        CHECK_LIKE_STD_UNARY(f, integers)
        CHECK_LIKE_STD(exp2)
        CHECK_LIKE_STD(expm1)
        CHECK_LIKE_STD(log2)
        CHECK_LIKE_STD(log1p)
        CHECK_LIKE_STD(cbrt)
        CHECK_LIKE_STD(asinh)
        CHECK_LIKE_STD(acosh)
        CHECK_LIKE_STD(atanh)
        CHECK_LIKE_STD(erf)
        CHECK_LIKE_STD(erfc)
        CHECK_LIKE_STD(tgamma)
        CHECK_LIKE_STD(lgamma)
        CHECK_LIKE_STD(ceil)
        CHECK_LIKE_STD(floor)
        CHECK_LIKE_STD(trunc)
        CHECK_LIKE_STD(round)
        CHECK_LIKE_STD(nearbyint)
        CHECK_LIKE_STD(rint)
        CHECK_LIKE_STD(fabs)
        CHECK_LIKE_STD(logb)
        CHECK_LIKE_STD(isfinite)
        CHECK_LIKE_STD(isinf)
        CHECK_LIKE_STD(isnan)
        CHECK_LIKE_STD(isnormal)
        CHECK_LIKE_STD(signbit)
        CHECK_LIKE_STD(fpclassify)
#       undef CHECK_LIKE_STD
    }
    SUBCASE("unary functions with an integer result")
    {
        // Infinities and NaNs have no integer value: the std:: functions
        // give an unspecified result for them.
#       define CHECK_LIKE_STD(f)                \
        CHECK_LIKE_STD_UNARY(f, finite)         \
        CHECK_LIKE_STD_UNARY(f, integers)
        CHECK_LIKE_STD(lround)
        CHECK_LIKE_STD(lrint)
        CHECK_LIKE_STD(llround)
        CHECK_LIKE_STD(llrint)
        CHECK_LIKE_STD(ilogb)
#       undef CHECK_LIKE_STD
    }
    SUBCASE("binary functions")
    {
            std::remove_cvref_t <decltype (values)>
        other;
        for (std::size_t i = 0; i < values.size (); ++i)
        {
            other[i] = values[(i + 5) % values.size ()];
        }
            auto const
        other_integers = opaque (array_t <int, 5> { 2, -1, 3, 0, 1 });
#       define CHECK_LIKE_STD(f)                            \
        CHECK_LIKE_STD_BINARY(f, values, other)             \
        CHECK_LIKE_STD_BINARY(f, integers, other_integers)
        CHECK_LIKE_STD(fmod)
        CHECK_LIKE_STD(remainder)
        CHECK_LIKE_STD(fmin)
        CHECK_LIKE_STD(fmax)
        CHECK_LIKE_STD(fdim)
        CHECK_LIKE_STD(copysign)
        CHECK_LIKE_STD(nextafter)
        CHECK_LIKE_STD(isgreater)
        CHECK_LIKE_STD(isgreaterequal)
        CHECK_LIKE_STD(isless)
        CHECK_LIKE_STD(islessequal)
        CHECK_LIKE_STD(islessgreater)
        CHECK_LIKE_STD(isunordered)
        CHECK_LIKE_STD(hypot)
#       undef CHECK_LIKE_STD
    }
    SUBCASE("binary functions with a specific second argument type")
    {
            auto const
        exponents = opaque (array_t <int, 13> { -3, -2, -1, 0, 1, 2, 3, -3, -2, -1, 0, 1, 2 });
            auto const
        long_exponents = opaque (array_t <long, 13> { -3, -2, -1, 0, 1, 2, 3, -3, -2, -1, 0, 1, 2 });
            auto const
        directions = opaque (array_t <long double, 13> { -1.0L, 0.0L, 1.0L, -1.0L, 0.0L, 1.0L, -1.0L, 0.0L, 1.0L, -1.0L, 0.0L, 1.0L, 2.0L });
            auto const
        integer_exponents = opaque (array_t <int, 5> { 2, -1, 3, 0, 1 });
            auto const
        integer_long_exponents = opaque (array_t <long, 5> { 2, -1, 3, 0, 1 });
            auto const
        integer_directions = opaque (array_t <long double, 5> { -1.0L, 0.0L, 1.0L, 2.0L, -2.0L });
        CHECK_LIKE_STD_BINARY(ldexp,      finite,   exponents)
        CHECK_LIKE_STD_BINARY(ldexp,      integers, integer_exponents)
        CHECK_LIKE_STD_BINARY(scalbn,     finite,   exponents)
        CHECK_LIKE_STD_BINARY(scalbn,     integers, integer_exponents)
        CHECK_LIKE_STD_BINARY(scalbln,    finite,   long_exponents)
        CHECK_LIKE_STD_BINARY(scalbln,    integers, integer_long_exponents)
        CHECK_LIKE_STD_BINARY(nexttoward, finite,   directions)
        CHECK_LIKE_STD_BINARY(nexttoward, integers, integer_directions)
    }
    SUBCASE("ternary functions")
    {
            std::remove_cvref_t <decltype (values)>
        y;
            std::remove_cvref_t <decltype (values)>
        z;
        for (std::size_t i = 0; i < values.size (); ++i)
        {
            y[i] = values[(i + 5) % values.size ()];
            z[i] = values[(i + 11) % values.size ()];
        }
            auto const
        integers_y = opaque (array_t <int, 5> { 2, -1, 3, 0, 1 });
            auto const
        integers_z = opaque (array_t <int, 5> { 0, 3, -2, 1, -1 });
#       define CHECK_LIKE_STD(f)                                    \
        CHECK_LIKE_STD_TERNARY(f, values, y, z)                     \
        CHECK_LIKE_STD_TERNARY(f, integers, integers_y, integers_z)
        CHECK_LIKE_STD(fma)
        CHECK_LIKE_STD(lerp)
        CHECK_LIKE_STD(hypot)
#       undef CHECK_LIKE_STD
    }
}
#undef CHECK_LIKE_STD_UNARY
#undef CHECK_LIKE_STD_BINARY
#undef CHECK_LIKE_STD_TERNARY

    template <class X, class Y, class Z>
    concept
has_fma = requires (X x, Y y, Z z) { calculisto::array::fma (x, y, z); };
    template <class A>
    concept
has_one_argument_hypot = requires (A a) { calculisto::array::hypot (a); };

TEST_CASE("arguments of the functions of several arguments")
{
        using
    a2 = array_t <double, 2>;
        using
    a3 = array_t <double, 3>;
    SUBCASE("at least one array, all of the same size")
    {
        static_assert (has_fma <a2, a2, a2>);
        static_assert (has_fma <a2, double, int>);
        static_assert (has_fma <double, double, a2>);
        static_assert (has_fma <array_t <double, 0>, double, double>);
        static_assert (not has_fma <double, double, double>);
        static_assert (not has_fma <a2, a3, double>);
        static_assert (not has_fma <a2, double, a3>);
        static_assert (not has_fma <array_t <std::string, 2>, a2, a2>);
        static_assert (std::is_same_v <decltype (fma (array_t <int, 0> {}, 1, 2)), array_t <double, 0>>);
    }
    SUBCASE("hypot of a single array is euclidean_norm")
    {
        // As for std::hypot, hypot takes 2 or 3 arguments, element-wise.
        static_assert (not has_one_argument_hypot <a3>);
        static_assert (std::is_same_v <decltype (euclidean_norm (a3 {})), double>);
        static_assert (std::is_same_v <decltype (hypot (a3 {}, a3 {})), a3>);
        static_assert (std::is_same_v <decltype (hypot (a3 {}, a3 {}, a3 {})), a3>);
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
    // Only a 2-argument hypot, like auto_diff's dual_t. Not a real hypot:
    // it adds, which shows how the elements are combined.
    constexpr x_t hypot (x_t a, x_t b) { return { a.v + b.v }; }
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
    SUBCASE("euclidean_norm chains the 2-argument hypot of a user type")
    {
            using
        user::x_t;
        CHECK(euclidean_norm (array_t <x_t, 1> { x_t { 1.0 } }).v == 1.0);
        CHECK(euclidean_norm (array_t <x_t, 2> { x_t { 1.0 }, x_t { 2.0 } }).v == 3.0);
        CHECK(euclidean_norm (array_t <x_t, 3> { x_t { 1.0 }, x_t { 2.0 }, x_t { 3.0 } }).v == 6.0);
        CHECK(euclidean_norm (array_t <x_t, 4> { x_t { 1.0 }, x_t { 2.0 }, x_t { 3.0 }, x_t { 4.0 } }).v == 10.0);
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

#define VALID(name, ...)                                              \
    template <class A, class B>                                       \
    concept                                                           \
name = requires (A a, B b) { __VA_ARGS__; };
VALID(add_assignable, a += b)
VALID(mul_assignable, a *= b)
VALID(mod_assignable, a %= b)
#undef VALID
    template <class A>
    concept
has_abs = requires (A a) { abs (a); };
    template <class A>
    concept
has_euclidean_norm = requires (A a) { euclidean_norm (a); };
    template <class A>
    concept
has_sum = requires (A a) { sum (a); };

// Asking whether an invalid operation exists must answer "no", not fail to
// compile, and must not answer "yes" for an operation that fails to compile.
TEST_CASE("operations exist only when they are valid")
{
        using
    d_t = array_t <double, 2>;
        using
    i_t = array_t <int, 2>;
        using
    s_t = array_t <std::string, 2>;
        using
    p_t = array_t <int*, 2>;
    SUBCASE("computed assignment with a scalar")
    {
        static_assert (add_assignable <d_t&, double>);
        static_assert (mod_assignable <i_t&, int>);
        static_assert (add_assignable <s_t&, std::string>);
        static_assert (not mod_assignable <d_t&, double>);
        static_assert (not mul_assignable <s_t&, std::string>);
    }
    SUBCASE("computed assignment with an array")
    {
        static_assert (add_assignable <d_t&, d_t>);
        static_assert (add_assignable <d_t&, i_t>);
        static_assert (mod_assignable <i_t&, i_t>);
        static_assert (add_assignable <s_t&, s_t>);
        static_assert (not mod_assignable <d_t&, d_t>);
        static_assert (not add_assignable <p_t&, p_t>);
        static_assert (not mul_assignable <s_t&, s_t>);
    }
    SUBCASE("math functions")
    {
        static_assert (has_abs <d_t>);
        static_assert (has_abs <i_t>);
        static_assert (not has_abs <s_t>);
        // std::abs is ambiguous for unsigned integers.
        static_assert (not has_abs <array_t <unsigned, 2>>);
        static_assert (has_euclidean_norm <d_t>);
        static_assert (has_euclidean_norm <array_t <double, 3>>);
        static_assert (has_euclidean_norm <array_t <double, 4>>);
        static_assert (not has_euclidean_norm <s_t>);
        static_assert (not has_euclidean_norm <array_t <std::string, 3>>);
        static_assert (not has_euclidean_norm <array_t <std::string, 4>>);
        static_assert (has_sum <d_t>);
        static_assert (has_sum <s_t>);
        static_assert (not has_sum <p_t>);
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
