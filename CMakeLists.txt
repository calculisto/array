cmake_minimum_required (VERSION 3.25)

project (calculisto_array
    VERSION 0.1.0
    DESCRIPTION "A C++20 array type with arithmetic operators and math functions"
    LANGUAGES CXX
)

# The library: header-only.
add_library (calculisto_array INTERFACE)
add_library (calculisto::array ALIAS calculisto_array)
target_include_directories (calculisto_array INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features (calculisto_array INTERFACE cxx_std_20)

# The tests, only when this is the main project, not when it is used by
# another one (add_subdirectory, FetchContent).
if (PROJECT_IS_TOP_LEVEL)
    include (CTest)
    if (BUILD_TESTING)
        add_subdirectory (tests)
    endif ()
endif ()
