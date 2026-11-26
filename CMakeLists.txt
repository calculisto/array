cmake_minimum_required (VERSION 3.25)

project (calculisto_array
    VERSION 0.1.1
    DESCRIPTION "A C++20 array type with arithmetic operators and math functions"
    LANGUAGES CXX
)

include (GNUInstallDirs)

# The library: header-only.
add_library (calculisto_array INTERFACE)
add_library (calculisto::array ALIAS calculisto_array)
set_target_properties (calculisto_array PROPERTIES EXPORT_NAME array)
target_include_directories (calculisto_array INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features (calculisto_array INTERFACE cxx_std_20)

# Installation, for use with find_package (calculisto_array): by default only
# when this is the main project, not when it is used by another one
# (add_subdirectory, FetchContent).
option (CALCULISTO_ARRAY_INSTALL "Generate the installation rules" ${PROJECT_IS_TOP_LEVEL})
if (CALCULISTO_ARRAY_INSTALL)
    include (CMakePackageConfigHelpers)
    # Header-only, so independent of the architecture: under share/.
    set (config_directory "${CMAKE_INSTALL_DATADIR}/cmake/calculisto_array")
    install (TARGETS calculisto_array EXPORT calculisto_array_targets)
    install (DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install (EXPORT calculisto_array_targets
        NAMESPACE calculisto::
        FILE calculisto_arrayTargets.cmake
        DESTINATION "${config_directory}"
    )
    configure_package_config_file (
        cmake/calculisto_arrayConfig.cmake.in
        "${PROJECT_BINARY_DIR}/calculisto_arrayConfig.cmake"
        INSTALL_DESTINATION "${config_directory}"
    )
    # Version 0.x: a new minor version may break the API.
    write_basic_package_version_file (
        "${PROJECT_BINARY_DIR}/calculisto_arrayConfigVersion.cmake"
        COMPATIBILITY SameMinorVersion
        ARCH_INDEPENDENT
    )
    install (FILES
        "${PROJECT_BINARY_DIR}/calculisto_arrayConfig.cmake"
        "${PROJECT_BINARY_DIR}/calculisto_arrayConfigVersion.cmake"
        DESTINATION "${config_directory}"
    )
endif ()

# The tests, only when this is the main project.
if (PROJECT_IS_TOP_LEVEL)
    include (CTest)
    if (BUILD_TESTING)
        add_subdirectory (tests)
    endif ()
endif ()
