cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(cpp_monads)

set(WFLAGS "-Werror -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wenum-compare -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-promo")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WFLAGS} -Wno-c++98-compat")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -march=native -mtune=native -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_TSAN "${CMAKE_CXX_FLAGS} -fsanitize=thread -fno-sanitize-recover=all -g -O0")
set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS} -Wodr -fsanitize=address,undefined -fno-sanitize=vptr -fno-sanitize-recover=all -g -O0")
set(CMAKE_CXX_FLAGS_RELASAN "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-sanitize=vptr -fno-sanitize-recover=all -g -O3 -march=native -mtune=native -fno-omit-frame-pointer")

find_package(GTest REQUIRED)
enable_testing()

set(TEST_DIR "tests")

add_library(monads INTERFACE)
target_include_directories(monads INTERFACE include)

set(TEST_SOURCES "${TEST_DIR}/tests.cpp")
add_executable(monads_tests ${TEST_SOURCES})
add_test(monads_tests monads_tests)
set_target_properties(monads_tests PROPERTIES EXCLUDE_FROM_ALL 1)
target_link_libraries(monads_tests PUBLIC monads gtest gtest_main)
