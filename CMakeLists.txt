cmake_minimum_required(VERSION 3.5)
project(clessc)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_subdirectory(libless)

add_executable(clessc src/lessc.cpp)
target_include_directories(clessc PRIVATE src .)
target_link_libraries(clessc less)

target_compile_definitions(clessc PRIVATE PACKAGE_STRING="LESS CSS Compiler")
target_compile_definitions(clessc PRIVATE PACKAGE_NAME="clessc")
target_compile_definitions(clessc PRIVATE PACKAGE_URL="https://github.com/BramvdKroef/clessc")
target_compile_definitions(clessc PRIVATE PACKAGE_BUGREPORT="bram@vanderkroef.net")

install(TARGETS clessc DESTINATION bin)
# TODO separate headers and sources to install library more easily

enable_testing()
find_package(GTest)
if (GTest_FOUND)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    set(testlessc_SOURCES
            tests/CssParser_test.cpp
            tests/CssTokenizer_test.cpp
            tests/CssSelectorParser_test.cpp
            tests/LessRuleset_test.cpp
            tests/LessSelectorParser_test.cpp
            tests/LessParser_test.cpp
            tests/ValueProcessor_test.cpp
            tests/Color_test.cpp
            )

    add_executable(testlessc ${testlessc_SOURCES})
    target_include_directories(testlessc PRIVATE src ${GTEST_INCLUDE_DIRS})
    target_link_libraries(testlessc less ${GTEST_BOTH_LIBRARIES} Threads::Threads)
    add_test(lesscTest testlessc)
else (GTest_FOUND)
    message(WARNING "Did not find libgtest. Unable to test.")
endif (GTest_FOUND)
