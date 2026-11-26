# doctest: the installed one if it is recent enough, otherwise downloaded.
include (FetchContent)
set (DOCTEST_NO_INSTALL ON)
# Our tests.cpp provides main: don't build doctest's library that does.
set (DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF)
FetchContent_Declare (doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.5.3
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS 2.5
)
FetchContent_MakeAvailable (doctest)

add_executable (tests tests.cpp test_array.cpp)
target_link_libraries (tests PRIVATE calculisto::array doctest::doctest)
set_target_properties (tests PROPERTIES CXX_EXTENSIONS OFF)

# The tests are always built with warnings as errors, and run with the
# address and undefined behavior sanitizers (only the address sanitizer on
# Windows, which has no undefined behavior sanitizer with MSVC, and a limited
# one with clang-cl).
set (gnu      "$<CXX_COMPILER_FRONTEND_VARIANT:GNU>")
set (msvc     "$<CXX_COMPILER_ID:MSVC>")
set (clang_cl "$<AND:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>")
target_compile_options (tests PRIVATE
    "$<${gnu}:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow;-Werror>"
    "$<${gnu}:-fsanitize=address,undefined;-fno-sanitize-recover=all;-fno-omit-frame-pointer>"
    "$<${msvc}:/W4;/WX;/permissive-;/Zc:preprocessor;/Zc:__cplusplus;/utf-8>"
    "$<${msvc}:/fsanitize=address>"
    "$<${clang_cl}:/W4;/WX;-Wpedantic;-Wconversion;-Wshadow;/utf-8>"
    "$<${clang_cl}:/fsanitize=address>"
)
target_link_options (tests PRIVATE
    "$<${gnu}:-fsanitize=address,undefined>"
)

add_test (NAME array COMMAND tests)

# With MSVC, the address sanitizer's runtime library is a DLL next to the
# compiler, which must be found when the tests run. (The default MSVC debug
# flags include /RTC1, incompatible with the address sanitizer, and
# /INCREMENTAL, which the linker ignores with it: the msvc preset sets them
# without these.)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    cmake_path (GET CMAKE_CXX_COMPILER PARENT_PATH compiler_directory)
    set_property (TEST array PROPERTY
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${compiler_directory}"
    )
# With clang-cl, the address sanitizer's DLL is in clang's runtime directory.
# (clang-cl doesn't support the address sanitizer with the debug runtime
# library: the clang-cl preset uses the release one.)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    execute_process (
        COMMAND "${CMAKE_CXX_COMPILER}" /clang:-print-runtime-dir
        OUTPUT_VARIABLE runtime_directory
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    set_property (TEST array PROPERTY
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${runtime_directory}"
    )
endif ()
