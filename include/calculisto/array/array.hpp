// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <array>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstddef>
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
// The scalar is captured by value: it may be an element of this array itself
// (a += a[0]), which the first iterations would otherwise modify.
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

// Tuple protocol, as for std::array, which enables structured bindings
// (with the std::tuple_size and std::tuple_element specializations at the
// end of this file). Unlike for std::array, std::get and std::apply can't
// be supported, as functions can't be added to namespace std: use
// unqualified get, or a.content, which is a std::array.
    template <std::size_t I, class T, std::size_t N>
    requires (I < N)
    constexpr T&
get (array_t <T, N>& a) noexcept
{
    return a[I];
}
    template <std::size_t I, class T, std::size_t N>
    requires (I < N)
    constexpr T const&
get (array_t <T, N> const& a) noexcept
{
    return a[I];
}
    template <std::size_t I, class T, std::size_t N>
    requires (I < N)
    constexpr T&&
get (array_t <T, N>&& a) noexcept
{
    return std::move (a[I]);
}
    template <std::size_t I, class T, std::size_t N>
    requires (I < N)
    constexpr T const&&
get (array_t <T, N> const&& a) noexcept
{
    return std::move (a[I]);
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
        array_t <V, N>
    r;
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
    RESULT_TYPE(atan2)
    RESULT_TYPE(exp2)
    RESULT_TYPE(expm1)
    RESULT_TYPE(log2)
    RESULT_TYPE(log1p)
    RESULT_TYPE(cbrt)
    RESULT_TYPE(asinh)
    RESULT_TYPE(acosh)
    RESULT_TYPE(atanh)
    RESULT_TYPE(erf)
    RESULT_TYPE(erfc)
    RESULT_TYPE(tgamma)
    RESULT_TYPE(lgamma)
    RESULT_TYPE(ceil)
    RESULT_TYPE(floor)
    RESULT_TYPE(trunc)
    RESULT_TYPE(round)
    RESULT_TYPE(nearbyint)
    RESULT_TYPE(rint)
    RESULT_TYPE(lround)
    RESULT_TYPE(lrint)
    RESULT_TYPE(llround)
    RESULT_TYPE(llrint)
    RESULT_TYPE(fabs)
    RESULT_TYPE(logb)
    RESULT_TYPE(ilogb)
    RESULT_TYPE(isfinite)
    RESULT_TYPE(isinf)
    RESULT_TYPE(isnan)
    RESULT_TYPE(isnormal)
    RESULT_TYPE(signbit)
    RESULT_TYPE(fpclassify)
    RESULT_TYPE(fmod)
    RESULT_TYPE(remainder)
    RESULT_TYPE(fmin)
    RESULT_TYPE(fmax)
    RESULT_TYPE(fdim)
    RESULT_TYPE(copysign)
    RESULT_TYPE(nextafter)
    RESULT_TYPE(nexttoward)
    RESULT_TYPE(ldexp)
    RESULT_TYPE(scalbn)
    RESULT_TYPE(scalbln)
    RESULT_TYPE(isgreater)
    RESULT_TYPE(isgreaterequal)
    RESULT_TYPE(isless)
    RESULT_TYPE(islessequal)
    RESULT_TYPE(islessgreater)
    RESULT_TYPE(isunordered)
    RESULT_TYPE(abs)
    RESULT_TYPE(hypot)
    RESULT_TYPE(fma)
    RESULT_TYPE(lerp)
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
        array_t <U, N>                                 \
    r;                                                 \
    using std::f;                                      \
    std::transform (                                   \
            p.begin (),                                \
            p.end   (),                                \
            r.begin (),                                \
            [](auto e){ return f (e); }                \
    );                                                 \
    return r;                                          \
}
UNARY_FUNCTION(abs)
UNARY_FUNCTION(cos)
UNARY_FUNCTION(acos)
UNARY_FUNCTION(cosh)
UNARY_FUNCTION(sin)
UNARY_FUNCTION(asin)
UNARY_FUNCTION(sinh)
UNARY_FUNCTION(tan)
UNARY_FUNCTION(tanh)
UNARY_FUNCTION(atan)
UNARY_FUNCTION(exp)
UNARY_FUNCTION(log)
UNARY_FUNCTION(log10)
UNARY_FUNCTION(sqrt)
UNARY_FUNCTION(exp2)
UNARY_FUNCTION(expm1)
UNARY_FUNCTION(log2)
UNARY_FUNCTION(log1p)
UNARY_FUNCTION(cbrt)
UNARY_FUNCTION(asinh)
UNARY_FUNCTION(acosh)
UNARY_FUNCTION(atanh)
UNARY_FUNCTION(erf)
UNARY_FUNCTION(erfc)
UNARY_FUNCTION(tgamma)
UNARY_FUNCTION(lgamma)
UNARY_FUNCTION(ceil)
UNARY_FUNCTION(floor)
UNARY_FUNCTION(trunc)
UNARY_FUNCTION(round)
UNARY_FUNCTION(nearbyint)
UNARY_FUNCTION(rint)
UNARY_FUNCTION(lround)
UNARY_FUNCTION(lrint)
UNARY_FUNCTION(llround)
UNARY_FUNCTION(llrint)
UNARY_FUNCTION(fabs)
UNARY_FUNCTION(logb)
UNARY_FUNCTION(ilogb)
UNARY_FUNCTION(isfinite)
UNARY_FUNCTION(isinf)
UNARY_FUNCTION(isnan)
UNARY_FUNCTION(isnormal)
UNARY_FUNCTION(signbit)
UNARY_FUNCTION(fpclassify)
#undef UNARY_FUNCTION

// Binary math functions, applied element-wise: to an array and a scalar, in
// either order, or to two arrays.
#define BINARY_FUNCTION(f)                                            \
    template <                                                        \
          class T                                                     \
        , class U                                                     \
        , std::size_t N                                               \
    >                                                                 \
    requires requires { typename detail::math::f##_result_t <T, U>; } \
    constexpr auto                                                    \
f (array_t <T, N> const& a, U b)                                      \
{                                                                     \
        using                                                         \
    V = detail::math::f##_result_t <T, U>;                            \
        array_t <V, N>                                                \
    r;                                                                \
    using std::f;                                                     \
    std::transform (                                                  \
            a.begin (),                                               \
            a.end   (),                                               \
            r.begin (),                                               \
            [=](auto x){ return f (x, b); }                           \
    );                                                                \
    return r;                                                         \
}                                                                     \
    template <                                                        \
          class T                                                     \
        , class U                                                     \
        , std::size_t N                                               \
    >                                                                 \
    requires requires { typename detail::math::f##_result_t <T, U>; } \
    constexpr auto                                                    \
f (T a, array_t <U, N> const& b)                                      \
{                                                                     \
        using                                                         \
    V = detail::math::f##_result_t <T, U>;                            \
        array_t <V, N>                                                \
    r;                                                                \
    using std::f;                                                     \
    std::transform (                                                  \
            b.begin (),                                               \
            b.end   (),                                               \
            r.begin (),                                               \
            [=](auto y){ return f (a, y); }                           \
    );                                                                \
    return r;                                                         \
}                                                                     \
    template <                                                        \
          class T                                                     \
        , class U                                                     \
        , std::size_t N                                               \
    >                                                                 \
    requires requires { typename detail::math::f##_result_t <T, U>; } \
    constexpr auto                                                    \
f (array_t <T, N> const& a, array_t <U, N> const& b)                  \
{                                                                     \
        using                                                         \
    V = detail::math::f##_result_t <T, U>;                            \
        array_t <V, N>                                                \
    r;                                                                \
    using std::f;                                                     \
    std::transform (                                                  \
            a.begin (),                                               \
            a.end   (),                                               \
            b.begin (),                                               \
            r.begin (),                                               \
            [=](auto x, auto y){ return f (x, y); }                   \
    );                                                                \
    return r;                                                         \
}
BINARY_FUNCTION(pow)
BINARY_FUNCTION(atan2)
BINARY_FUNCTION(fmod)
BINARY_FUNCTION(remainder)
BINARY_FUNCTION(fmin)
BINARY_FUNCTION(fmax)
BINARY_FUNCTION(fdim)
BINARY_FUNCTION(copysign)
BINARY_FUNCTION(nextafter)
BINARY_FUNCTION(nexttoward)
BINARY_FUNCTION(ldexp)
BINARY_FUNCTION(scalbn)
BINARY_FUNCTION(scalbln)
BINARY_FUNCTION(isgreater)
BINARY_FUNCTION(isgreaterequal)
BINARY_FUNCTION(isless)
BINARY_FUNCTION(islessequal)
BINARY_FUNCTION(islessgreater)
BINARY_FUNCTION(isunordered)
BINARY_FUNCTION(hypot)
#undef BINARY_FUNCTION

// Functions of several arguments, each of which can be an array or a scalar,
// applied element-wise: at least one argument must be an array, and all the
// arrays must have the same size.
    namespace
detail
{
        template <class X>
        struct
    is_array_t : std::false_type {};
        template <class T, std::size_t N>
        struct
    is_array_t <array_t <T, N>> : std::true_type {};

    // The element type of an array, or the type of a scalar.
        template <class X>
        struct
    element_type { using type = X; };
        template <class T, std::size_t N>
        struct
    element_type <array_t <T, N>> { using type = T; };
        template <class X>
        using
    element_type_t = typename element_type <X>::type;

    // The size of an array, 0 for a scalar.
        template <class X>
        inline constexpr std::size_t
    size_of = 0;
        template <class T, std::size_t N>
        inline constexpr std::size_t
    size_of <array_t <T, N>> = N;

        template <class... X>
        inline constexpr std::size_t
    common_size = std::max ({ size_of <X>... });

        template <class... X>
        concept
    elementwise_arguments =
            (is_array_t <X>::value or ...)
        and ((not is_array_t <X>::value or size_of <X> == common_size <X...>) and ...);

    // The i-th element of an array, or the scalar itself.
        template <class X>
        constexpr decltype (auto)
    element (X const& x, std::size_t i)
    {
        if constexpr (is_array_t <X>::value)
        {
            return x[i];
        }
        else
        {
            return x;
        }
    }
} // namespace detail

#define TERNARY_FUNCTION(f)                                                                \
    template <class X, class Y, class Z>                                                   \
    requires                                                                               \
            detail::elementwise_arguments <X, Y, Z>                                        \
        and requires                                                                       \
        {                                                                                  \
            typename detail::math::f##_result_t <                                          \
                  detail::element_type_t <X>                                               \
                , detail::element_type_t <Y>                                               \
                , detail::element_type_t <Z>                                               \
            >;                                                                             \
        }                                                                                  \
    constexpr auto                                                                         \
f (X const& x, Y const& y, Z const& z)                                                     \
{                                                                                          \
        constexpr auto                                                                     \
    N = detail::common_size <X, Y, Z>;                                                     \
        using                                                                              \
    V = detail::math::f##_result_t <                                                       \
          detail::element_type_t <X>                                                       \
        , detail::element_type_t <Y>                                                       \
        , detail::element_type_t <Z>                                                       \
    >;                                                                                     \
        array_t <V, N>                                                                     \
    r;                                                                                     \
    using std::f;                                                                          \
    for (std::size_t i = 0; i < N; ++i)                                                    \
    {                                                                                      \
        r[i] = f (detail::element (x, i), detail::element (y, i), detail::element (z, i)); \
    }                                                                                      \
    return r;                                                                              \
}
TERNARY_FUNCTION(fma)
TERNARY_FUNCTION(lerp)
TERNARY_FUNCTION(hypot)
#undef TERNARY_FUNCTION

// The Euclidean norm of an array: the square root of the sum of the squares of
// its elements. It chains the 2-argument hypot of the elements (or uses the
// 3-argument hypot for 3 elements), which avoids the overflow and underflow of
// the intermediate squares: summing the squares directly overflows as soon as
// the sum exceeds the largest value of the type, e.g. for doubles around
// 1e154, floats around 1e19, and ints whose squares sum above 2^31 - 1, like
// 30000 and 40000 (undefined behavior).
    template <class T, std::size_t N>
    requires requires
    {
        T {};
        typename detail::math::hypot_result_t <T, T>;
        typename detail::math::hypot_result_t <detail::math::hypot_result_t <T, T>, T>;
    }
    constexpr auto
euclidean_norm (array_t <T, N> const& p)
{
        using std::hypot;
        using
    R = detail::math::hypot_result_t <T, T>;
    if constexpr (N == 0)
    {
        return R {};
    }
    else if constexpr (N == 1)
    {
        return R { hypot (p[0], T {}) };
    }
    else
    {
            R
        r = hypot (p[0], p[1]);
        for (std::size_t i = 2; i < N; ++i)
        {
            r = hypot (r, p[i]);
        }
        return r;
    }
}
    template <class T>
    requires requires { typename detail::math::hypot_result_t <T, T, T>; }
    constexpr auto
euclidean_norm (array_t <T, 3> const& p)
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

    template <class T, std::size_t N>
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

// Tuple protocol, see get above.
    template <class T, std::size_t N>
    struct
std::tuple_size <calculisto::array::array_t <T, N>>
    : std::integral_constant <std::size_t, N>
{};
    template <std::size_t I, class T, std::size_t N>
    requires (I < N)
    struct
std::tuple_element <I, calculisto::array::array_t <T, N>>
{
        using
    type = T;
};
