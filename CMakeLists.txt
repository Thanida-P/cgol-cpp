cmake_minimum_required(VERSION 3.16)
project(CGOL)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

include_directories(include)
include_directories(assets)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# file(GLOB SRC src/*.cpp)
# file(GLOB TEST tests/*.cpp)

set(CMAKE_AUTOMOC ON)

add_executable(
    main
    src/main.cpp
    src/cgol.cpp
    src/parser_utils.cpp
    src/gui.cpp
)

add_executable(
    tests
    tests/tests.cpp
    src/cgol.cpp
    src/parser_utils.cpp
)

target_link_libraries(main Qt6::Core Qt6::Gui Qt6::Widgets)
target_include_directories(tests PRIVATE includes)

set(TARGETS main)

set_target_properties(
    ${TARGETS}
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

foreach(T IN LISTS TARGETS)
    if (MSVC)
        target_compile_options(${T} PRIVATE /Wall /WX)
    else()
        target_compile_options(${T} PRIVATE -Wall -Wextra)
    endif()
endforeach()