# calculisto::array
A C++20 array type with arithmetic operators and math functions.

## Dependencies
- A C++20 compiler.
- [CMake](https://cmake.org) 3.25 or later, to install the library or run the
  tests.
- To run the tests, the [doctest](https://github.com/doctest/doctest) library,
  version 2.5 or later. If it isn't installed, CMake downloads it.

## Installation
This is a header-only library. It can be used in any of these ways:
- copy the `include` directory where your compiler can find it;
- from a CMake project, with `add_subdirectory` or `FetchContent`, then link to
  the `calculisto::array` target;
- install it with CMake:
  ```
  cmake --preset dev
  cmake --install build/dev --prefix <directory>
  ```
  then, from a CMake project:
  ```cmake
  find_package (calculisto_array 0.1 REQUIRED)
  target_link_libraries (<target> PRIVATE calculisto::array)
  ```
  adding `<directory>` to `CMAKE_PREFIX_PATH` if it isn't a standard location.

## Tests
To configure, build and run the tests with the default compiler, execute
```
cmake --workflow --preset dev
```
in the root directory of the project. The tests are compiled with warnings as
errors, and run with the address sanitizer and, except with MSVC and clang-cl,
the undefined behavior sanitizer. They also check that the installed library
can be used with `find_package`.

Other presets select a compiler: `gcc`, `clang`, `clang-libcxx`,
`clang-libcxx-class-iterators`, and on Windows `msvc` and `clang-cl`.
`cmake --list-presets=workflow` lists them.

## License
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

## Affiliation
This material is developed by the Numerical Modelling platform at the
Institut des Sciences de la Terre d'Orléans (https://www.isto-orleans.fr),
a joint laboratory of the University of Orléans (https://www.univ-orleans.fr),
the french National Center For Scientific Research (https://www.cnrs.fr) and
the french Geological Survey (https://www.brgm.fr).

[![logo ISTO](https://calcul-isto.cnrs-orleans.fr/logos/isto-156.png)](https://www.isto-orleans.fr) &emsp;
[![logo CNRS](https://calcul-isto.cnrs-orleans.fr/logos/cnrs-128.png)](https://www.cnrs.fr) &emsp;
[![logo UO](https://calcul-isto.cnrs-orleans.fr/logos/uo-128.png)](https://www.univ-orleans.fr) &emsp;
[![logo BRGM](https://calcul-isto.cnrs-orleans.fr/logos/brgm-256.png)](https://www.brgm.fr)
