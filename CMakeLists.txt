cmake_minimum_required(VERSION 3.15)
project(blitz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build as a shared library" ON)

# Base source files (common)
set(BASE_SOURCES
        src/Provider.cpp
        src/Connection.cpp
)

add_library(blitz ${BASE_SOURCES})

target_include_directories(blitz PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (WIN32)
    message(STATUS "Configuring for Windows")
    target_compile_definitions(blitz PRIVATE PLATFORM_WINDOWS BLITZ_BUILD)
    target_link_libraries(blitz PUBLIC ws2_32)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux")
    target_compile_definitions(blitz PRIVATE PLATFORM_LINUX BLITZ_BUILD)
    target_link_libraries(blitz PRIVATE pthread)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Executable to test the library

add_executable(blitz_test src/main.cpp)
target_link_libraries(blitz_test PRIVATE blitz)

if (WIN32)
    target_compile_definitions(blitz_test PRIVATE PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(blitz_test PRIVATE PLATFORM_LINUX)
endif()

install(
        TARGETS blitz
        EXPORT blitzTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)