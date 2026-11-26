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
# Keep the warning flags of gcc and clang in sync with compile_flags.txt, which
# gives them to clangd.
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
    # Visual Studio links with lld-link directly, not through clang-cl, which
    # would add the address sanitizer's libraries itself: give them to the
    # linker, as clang-cl does with the release runtime library.
    set (asan_libraries
        "${runtime_directory}/clang_rt.asan_dynamic-x86_64.lib"
        "${runtime_directory}/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
    )
    foreach (library IN LISTS asan_libraries)
        if (NOT EXISTS "${library}")
            file (GLOB available "${runtime_directory}/clang_rt.asan*")
            list (JOIN available "\n    " available)
            message (FATAL_ERROR
                "Address sanitizer library not found: ${library}\n"
                "Available in ${runtime_directory}:\n    ${available}"
            )
        endif ()
    endforeach ()
    list (GET asan_libraries 0 asan_runtime)
    list (GET asan_libraries 1 asan_thunk)
    target_link_options (tests PRIVATE
        "${asan_runtime}"
        "/wholearchive:${asan_thunk}"
    )
endif ()

# The installation: install into the build directory, then build and run a
# project using the installed library through find_package.
if (CALCULISTO_ARRAY_INSTALL)
    set (test_prefix "${PROJECT_BINARY_DIR}/test-install")
    add_test (NAME install
        COMMAND "${CMAKE_COMMAND}" --install "${PROJECT_BINARY_DIR}"
            --prefix "${test_prefix}" --config $<CONFIG>
    )
    set_tests_properties (install PROPERTIES FIXTURES_SETUP installed)
    add_test (NAME consumer
        COMMAND "${CMAKE_CTEST_COMMAND}" -C $<CONFIG>
            --build-and-test
                "${CMAKE_CURRENT_SOURCE_DIR}/consumer"
                "${CMAKE_CURRENT_BINARY_DIR}/consumer"
            --build-generator "${CMAKE_GENERATOR}"
            --build-project calculisto_array_consumer
            --build-options "-DCMAKE_PREFIX_PATH=${test_prefix}"
            --test-command consumer
    )
    set_tests_properties (consumer PROPERTIES FIXTURES_REQUIRED installed)
endif ()
