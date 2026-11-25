// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <array>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

    namespace
calculisto::array
{
/** An array with arithmetic operations.
 * This is a std::array with operators and math functions.
 * (A bit like what std::valarray is to std::vector.)
 * Like std::array, it is an aggregate: it has no constructors, and narrowing
 * conversions in its initialization are rejected. To convert the elements to
 * another type, use array_cast.
 */
    template <class T, std::size_t N>
    struct
array_t
{
public:
        using
    backend_type = std::array <T, N>;

        backend_type
    content;

    // Use the backend's types: std::array's iterators are not necessarily
    // pointers (they are class types in some standard libraries).
        using
    value_type             = typename backend_type::value_type;
        using
    pointer                = typename backend_type::pointer;
        using
    const_pointer          = typename backend_type::const_pointer;
        using
    reference              = typename backend_type::reference;
        using
    const_reference        = typename backend_type::const_reference;
        using
    iterator               = typename backend_type::iterator;
        using
    const_iterator         = typename backend_type::const_iterator;
        using
    size_type              = typename backend_type::size_type;
        using
    difference_type        = typename backend_type::difference_type;
        using
    reverse_iterator       = typename backend_type::reverse_iterator;
        using
    const_reverse_iterator = typename backend_type::const_reverse_iterator;

        constexpr void
    fill (const value_type& u)
    {
        content.fill (u);
    }

        constexpr void
    swap (array_t& other) noexcept (std::is_nothrow_swappable_v <value_type>)
    {
        content.swap (other.content);
    }
    // Iterators.
        constexpr iterator
    begin () noexcept
    {
       return content.begin ();
    }

        constexpr const_iterator
    begin () const noexcept
    {
       return content.begin ();
    }

        constexpr iterator
    end () noexcept
    {
        return content.end ();
    }

        constexpr const_iterator
    end () const noexcept
    {
        return content.end ();
    }

        constexpr reverse_iterator
    rbegin () noexcept
    {
        return content.rbegin ();
    }

        constexpr const_reverse_iterator
    rbegin () const noexcept
    {
        return content.rbegin ();
    }

        constexpr reverse_iterator
    rend () noexcept
    {
        return content.rend ();
    }

        constexpr const_reverse_iterator
    rend () const noexcept
    {
        return content.rend ();
    }

        constexpr const_iterator
    cbegin () const noexcept
    {
        return content.cbegin ();
    }

        constexpr const_iterator
    cend () const noexcept
    {
        return content.cend ();
    }

        constexpr const_reverse_iterator
    crbegin () const noexcept
    {
        return content.crbegin ();
    }

        constexpr const_reverse_iterator
    crend () const noexcept
    {
        return content.crend ();
    }

    // Capacity.
        constexpr size_type
    size () const noexcept
    {
        return content.size ();
    }

        constexpr size_type
    max_size () const noexcept
    {
        return content.max_size ();
    }

        constexpr bool
    empty () const noexcept
    {
        return content.empty ();
    }

    // Element access.
        constexpr reference
    operator[] (size_type n) noexcept
    {
        return content[n];
    }

        constexpr const_reference
    operator[] (size_type n) const noexcept
    {
        return content[n];
    }

        constexpr reference
    at (size_type n)
    {
        return content.at (n);
    }

        constexpr const_reference
    at (size_type n) const
    {
        return content.at (n);
    }

        constexpr reference
    front () noexcept
    {
        return content.front ();
    }

        constexpr const_reference
    front () const noexcept
    {
        return content.front ();
    }

        constexpr reference
    back () noexcept
    {
        return content.back ();
    }

        constexpr const_reference
    back () const noexcept
    {
        return content.back ();
    }

        constexpr pointer
    data () noexcept
    {
        return content.data ();
    }

        constexpr const_pointer
    data () const noexcept
    {
        return content.data ();
    }

// The computed assignments exist only if they are valid for the elements.
#   define COMPUTED_ASSIGNMENT(op)                                            \
        constexpr array_t&                                                    \
    operator op##= (const value_type& x)                                      \
        requires requires (value_type& e, value_type const& y) { e op##= y; } \
    {                                                                         \
        std::transform (                                                      \
                begin (),                                                     \
                end (),                                                       \
                begin (),                                                     \
                [=](auto& e){ return e op##= x; }                             \
        );                                                                    \
        return *this;                                                         \
    }
    COMPUTED_ASSIGNMENT(*)
    COMPUTED_ASSIGNMENT(/)
    COMPUTED_ASSIGNMENT(%)
    COMPUTED_ASSIGNMENT(+)
    COMPUTED_ASSIGNMENT(-)
    COMPUTED_ASSIGNMENT(^)
    COMPUTED_ASSIGNMENT(&)
    COMPUTED_ASSIGNMENT(|)
    COMPUTED_ASSIGNMENT(<<)
    COMPUTED_ASSIGNMENT(>>)
#   undef COMPUTED_ASSIGNMENT

#   define COMPUTED_ASSIGNMENT(op)                                                       \
        template <                                                                       \
              class U                                                                    \
        >                                                                                \
        requires requires (value_type& e, U const& u) { e op##= u; }                     \
        constexpr array_t&                                                               \
    operator op##= (array_t <U, N> const& v)                                             \
    {                                                                                    \
        std::transform (                                                                 \
            begin   (),                                                                  \
            end     (),                                                                  \
            v.begin (),                                                                  \
            begin   (),                                                                  \
            [=](auto a, auto const& b){ return a op##= b; }                              \
        );                                                                               \
        return *this;                                                                    \
    }
    COMPUTED_ASSIGNMENT(*)
    COMPUTED_ASSIGNMENT(/)
    COMPUTED_ASSIGNMENT(%)
    COMPUTED_ASSIGNMENT(+)
    COMPUTED_ASSIGNMENT(-)
    COMPUTED_ASSIGNMENT(^)
    COMPUTED_ASSIGNMENT(&)
    COMPUTED_ASSIGNMENT(|)
    COMPUTED_ASSIGNMENT(<<)
    COMPUTED_ASSIGNMENT(>>)
#   undef COMPUTED_ASSIGNMENT

        auto
    operator <=> (array_t <T, N> const&) const = default;
}; // struct array_t

    template <class T, std::size_t N>
    constexpr void
swap (array_t <T, N>& a, array_t <T, N>& b) noexcept (noexcept (a.swap (b)))
{
    a.swap (b);
}

// As for std::array, all the elements must have the same type.
    template <class T, class... U>
    requires (std::is_same_v <T, U> && ...)
array_t (T, U...) -> array_t <T, 1 + sizeof... (U)>;
    template <
          class T
        , std::size_t N
    >
    requires requires (T const& x) { +x; }
    constexpr auto
operator + (array_t <T, N> const& a)
{
        using
    V = decltype (+a[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          a.begin ()
        , a.end   ()
        , r.begin ()
        , [](auto x){ return +x; }
    );
    return r;
}
    template <
          class T
        , std::size_t N
    >
    requires requires (T const& x) { -x; }
    constexpr auto
operator - (array_t <T, N> const& a)
{
        using
    V = decltype (-a[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          a.begin ()
        , a.end   ()
        , r.begin ()
        , [](auto x){ return -x; }
    );
    return r;
}

    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x + y; }
    constexpr auto
operator + (array_t <T, N> const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a[0] + b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          a.begin ()
        , a.end   ()
        , b.begin ()
        , r.begin ()
        , [](auto x, auto y){ return x + y; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x - y; }
    constexpr auto
operator - (array_t <T, N> const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a[0] - b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          a.begin ()
        , a.end   ()
        , b.begin ()
        , r.begin ()
        , [](auto x, auto y){ return x - y; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x * y; }
    constexpr auto
operator * (array_t <T, N> const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a[0] * b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          a.begin ()
        , a.end   ()
        , b.begin ()
        , r.begin ()
        , [](auto x, auto y){ return x * y; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x / y; }
    constexpr auto
operator / (array_t <T, N> const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a[0] / b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          a.begin ()
        , a.end   ()
        , b.begin ()
        , r.begin ()
        , [](auto x, auto y){ return x / y; }
    );
    return r;
}

    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x + y; }
    constexpr auto
operator + (array_t <T, N> const& a, U const& b)
{
        using
    V = decltype (a[0] + b);
        auto
    r = array_t <V, N> {};
    // TODO capture by value or by ref depending on U (using if constexpr)?
    // E.g. is U is trivially copyable?
    std::transform (
          a.begin ()
        , a.end   ()
        , r.begin ()
        , [&](auto x){ return x + b; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x + y; }
    constexpr auto
operator + (T const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a + b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          b.begin ()
        , b.end   ()
        , r.begin ()
        , [&](auto x){ return a + x; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x - y; }
    constexpr auto
operator - (array_t <T, N> const& a, U const& b)
{
        using
    V = decltype (a[0] - b);
        auto
    r = array_t <V, N> {};
    // TODO capture by value or by ref depending on U (using if constexpr)?
    // E.g. is U is trivially copyable?
    std::transform (
          a.begin ()
        , a.end   ()
        , r.begin ()
        , [&](auto x){ return x - b; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x - y; }
    constexpr auto
operator - (T const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a - b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          b.begin ()
        , b.end   ()
        , r.begin ()
        , [&](auto x){ return a - x; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x * y; }
    constexpr auto
operator * (array_t <T, N> const& a, U const& b)
{
        using
    V = decltype (a[0] * b);
        auto
    r = array_t <V, N> {};
    // TODO capture by value or by ref depending on U (using if constexpr)?
    // E.g. is U is trivially copyable?
    std::transform (
          a.begin ()
        , a.end   ()
        , r.begin ()
        , [&](auto x){ return x * b; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x * y; }
    constexpr auto
operator * (T const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a * b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          b.begin ()
        , b.end   ()
        , r.begin ()
        , [&](auto x){ return a * x; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x / y; }
    constexpr auto
operator / (array_t <T, N> const& a, U const& b)
{
        using
    V = decltype (a[0] / b);
        auto
    r = array_t <V, N> {};
    // TODO capture by value or by ref depending on U (using if constexpr)?
    // E.g. is U is trivially copyable?
    std::transform (
          a.begin ()
        , a.end   ()
        , r.begin ()
        , [&](auto x){ return x / b; }
    );
    return r;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires (T const& x, U const& y) { x / y; }
    constexpr auto
operator / (T const& a, array_t <U, N> const& b)
{
        using
    V = decltype (a / b[0]);
        auto
    r = array_t <V, N> {};
    std::transform (
          b.begin ()
        , b.end   ()
        , r.begin ()
        , [&](auto x){ return a / x; }
    );
    return r;
}

// Result types of the math functions applied to the elements.
    namespace
detail::math
{
    // The using-declarations make the standard functions visible to the
    // unqualified calls below, while argument-dependent lookup can still find
    // overloads for user types. They must not be put in calculisto::array,
    // where they would leak to the users of this header.
#   define RESULT_TYPE(f)                                    \
    using std::f;                                            \
        template <class... ArgsType>                         \
        using                                                \
    f##_result_t = decltype (f (std::declval <ArgsType> ()...));
    RESULT_TYPE(cos)
    RESULT_TYPE(acos)
    RESULT_TYPE(cosh)
    RESULT_TYPE(sin)
    RESULT_TYPE(asin)
    RESULT_TYPE(sinh)
    RESULT_TYPE(tan)
    RESULT_TYPE(tanh)
    RESULT_TYPE(atan)
    RESULT_TYPE(exp)
    RESULT_TYPE(log)
    RESULT_TYPE(log10)
    RESULT_TYPE(sqrt)
    RESULT_TYPE(pow)
    RESULT_TYPE(abs)
    RESULT_TYPE(hypot)
#   undef RESULT_TYPE
} // namespace detail::math

#define UNARY_FUNCTION(f)                              \
    template <                                         \
          class T                                      \
        , std::size_t N                                \
    >                                                  \
    requires requires                                  \
    {                                                  \
        typename detail::math::f##_result_t <T>;       \
    }                                                  \
    constexpr auto                                     \
f (array_t <T, N> const& p)                            \
{                                                      \
        using                                          \
    U = detail::math::f##_result_t <T>;                \
        auto                                           \
    r = array_t <U, N> {};                             \
    using std::f;                                      \
    std::transform (                                   \
            p.begin (),                                \
            p.end   (),                                \
            r.begin (),                                \
            [](auto e){ return f (e); }                \
    );                                                 \
    return r;                                          \
}
// See below.
// UNARY_FUNCTION(abs)
UNARY_FUNCTION(cos)
UNARY_FUNCTION(acos)
UNARY_FUNCTION(cosh)
UNARY_FUNCTION(sin)
UNARY_FUNCTION(asin)
UNARY_FUNCTION(sinh)
UNARY_FUNCTION(tan)
UNARY_FUNCTION(tanh)
UNARY_FUNCTION(atan)
// UNARY_FUNCTION(atan2)
UNARY_FUNCTION(exp)
UNARY_FUNCTION(log)
UNARY_FUNCTION(log10)
UNARY_FUNCTION(sqrt)
#undef UNARY_FUNCTION

/* Can't use the above macro because
 * both std::abs and ::abs exist and this make
 * the return type deduction fail.
 *
 */
    template <
          class T
        , std::size_t N
    >
    requires std::is_convertible_v <detail::math::abs_result_t <T>, T>
    constexpr auto
abs (array_t <T, N> p)
{
    using std::abs;
    std::transform (
            p.begin (),
            p.end   (),
            p.begin (),
            [=](auto e){ return abs (e); }
    );
    return p;
}
    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires { typename detail::math::pow_result_t <T, U>; }
    constexpr auto
pow (array_t <T, N> const& p, U exp)
{
        using
    V = detail::math::pow_result_t <T, U>;
        auto
    r = array_t <V, N> {};
    using std::pow;
    std::transform (
            p.begin (),
            p.end   (),
            r.begin (),
            [=](auto e){ return pow (e, exp); }
    );
    return r;
}

    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires { typename detail::math::pow_result_t <T, U>; }
    constexpr auto
pow (T t, array_t <U, N> const& exp)
{
        using
    V = detail::math::pow_result_t <T, U>;
        auto
    r = array_t <V, N> {};
    using std::pow;
    std::transform (
            exp.begin (),
            exp.end   (),
            r.begin (),
            [=](auto ee){ return pow (t, ee); }
    );
    return r;
}

    template <
          class T
        , class U
        , std::size_t N
    >
    requires requires { typename detail::math::pow_result_t <T, U>; }
    constexpr auto
pow (array_t <T, N> const& p, array_t <U, N> const& exp)
{
        using
    V = detail::math::pow_result_t <T, U>;
        auto
    r = array_t <V, N> {};
    using std::pow;
    std::transform (
            p.begin (),
            p.end   (),
            exp.begin (),
            r.begin (),
            [=](auto e, auto ee){ return pow (e, ee); }
    );
    return r;
}
    template <class T, std::size_t N>
    requires requires (T t)
    {
        t = t + t * t;
        T {};
        typename detail::math::sqrt_result_t <T>;
    }
    constexpr auto
hypot (array_t <T, N> const& p)
{
        using std::sqrt;
        auto const
    squares = p * p;
    return sqrt (std::accumulate (
        std::begin (squares),
        std::end   (squares),
        T {}
    ));
}
    template <class T>
    requires requires { typename detail::math::hypot_result_t <T, T>; }
    constexpr auto
hypot (array_t <T, 2> const& p)
{
        using std::hypot;
    return hypot (p[0], p[1]);
}
    template <class T>
    requires requires { typename detail::math::hypot_result_t <T, T, T>; }
    constexpr auto
hypot (array_t <T, 3> const& p)
{
        using std::hypot;
    return hypot (p[0], p[1], p[2]);
}

// Concatenation.
    namespace
detail
{
        template <
              class T
            , std::size_t    N1
            , std::size_t... I1
            , std::size_t    N2
            , std::size_t... I2
        >
        constexpr auto
    concatenate (
              array_t <T, N1> const& a1
            , array_t <T, N2> const& a2
            , std::index_sequence <I1...>
            , std::index_sequence <I2...>
    ){
        return array_t <T, N1 + N2> { a1[I1]..., a2[I2]... };
    }
} // namespace detail
    template <
          class T
        , std::size_t N1
        , std::size_t N2
    >
    constexpr array_t <T, N1 + N2>
concatenate (
          array_t <T, N1> const& a1
        , array_t <T, N2> const& a2
){
    return detail::concatenate (a1, a2, std::make_index_sequence <N1> (), std::make_index_sequence <N2> ());
}
    template <class T, std::size_t N>
    constexpr array_t <T, N>
concatenate (array_t <T, N> const& a)
{
    return a;
}
    template <
          class T
        , std::size_t N1
        , std::size_t N2
        , std::size_t N3
        , class... Arrays
    >
    constexpr auto
concatenate (
          array_t <T, N1> const& a1
        , array_t <T, N2> const& a2
        , array_t <T, N3> const& a3
        , Arrays const&... arrays
){
    return concatenate (concatenate (a1, a2), a3, arrays...);
}

// Conversion of the elements to another type, as with static_cast.
    namespace
detail
{
        template <
              class U
            , class T
            , std::size_t N
            , std::size_t... I
        >
        constexpr array_t <U, N>
    array_cast (array_t <T, N> const& a, std::index_sequence <I...>)
    {
        return array_t <U, N> { static_cast <U> (a[I])... };
    }
} // namespace detail
    template <
          class U
        , class T
        , std::size_t N
    >
    requires requires (T const& x) { static_cast <U> (x); }
    constexpr array_t <U, N>
array_cast (array_t <T, N> const& a)
{
    return detail::array_cast <U> (a, std::make_index_sequence <N> ());
}

    template <class T, auto N>
    requires requires (T t)
    {
        t = t + t;
        T {};
    }
    constexpr auto
sum (array_t <T, N> const& array)
{
    return std::accumulate (
          array.begin ()
        , array.end ()
        , T {}
    );
}


} // namespace calculisto::array
