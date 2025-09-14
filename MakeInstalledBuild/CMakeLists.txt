cmake_minimum_required(VERSION 3.16)

# Project configuration
project(UERegistryTool 
    VERSION 1.0.0
    DESCRIPTION "Unreal Engine Registry Build Tool"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific configuration
if(NOT WIN32)
    message(FATAL_ERROR "This application is designed for Windows only")
endif()

# Create the executable
add_executable(${PROJECT_NAME} main.cpp)

# Set target properties for Windows
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE FALSE  # Keep as console application
    OUTPUT_NAME "UERegistryTool"
)

# Compiler-specific options
if(MSVC)
    # Enable Unicode support
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        UNICODE 
        _UNICODE
        WIN32_LEAN_AND_MEAN
    )
    
    # Set warning level
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    
    # Runtime library (optional: use static runtime)
    # set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# MinGW/GCC specific options
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        UNICODE 
        _UNICODE
        WIN32_LEAN_AND_MEAN
    )
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Installation rules (optional)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Debug configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG _DEBUG)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
endif()

# Custom build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "" FORCE)