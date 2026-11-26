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
# address and undefined behavior sanitizers (only the address sanitizer with
# MSVC, which has no undefined behavior sanitizer).
set (gnu   "$<CXX_COMPILER_FRONTEND_VARIANT:GNU>")
set (msvc  "$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>")
target_compile_options (tests PRIVATE
    "$<${gnu}:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow;-Werror>"
    "$<${gnu}:-fsanitize=address,undefined;-fno-sanitize-recover=all;-fno-omit-frame-pointer>"
    "$<${msvc}:/W4;/WX;/permissive-;/Zc:preprocessor;/Zc:__cplusplus;/utf-8>"
    "$<${msvc}:/fsanitize=address>"
)
target_link_options (tests PRIVATE
    "$<${gnu}:-fsanitize=address,undefined>"
)

add_test (NAME array COMMAND tests)
