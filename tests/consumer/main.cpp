#include <calculisto/array/array.hpp>
    using calculisto::array::array_t;

    int
main ()
{
        constexpr
        array_t
    a { 1, 2, 3 };
    static_assert (sum (a) == 6);
    return sum (a * 2) == 12 ? 0 : 1;
}
