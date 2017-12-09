cmake_minimum_required(VERSION 3.5)
project(clessc)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(lessc_SOURCES
        src/css/CssParser.cpp
        src/css/CssPrettyWriter.cpp
        src/css/CssTokenizer.cpp
        src/css/CssWriter.cpp
        src/css/ParseException.cpp
        src/css/SourceMapWriter.cpp
        src/less/LessParser.cpp
        src/less/LessTokenizer.cpp
        src/lessstylesheet/Closure.cpp
        src/lessstylesheet/Extension.cpp
        src/lessstylesheet/Function.cpp
        src/lessstylesheet/LessAtRule.cpp
        src/lessstylesheet/LessMediaQuery.cpp
        src/lessstylesheet/LessRuleset.cpp
        src/lessstylesheet/LessSelector.cpp
        src/lessstylesheet/LessStylesheet.cpp
        src/lessstylesheet/MediaQueryRuleset.cpp
        src/lessstylesheet/Mixin.cpp
        src/lessstylesheet/MixinCall.cpp
        src/lessstylesheet/ProcessingContext.cpp
        src/lessstylesheet/UnprocessedStatement.cpp
        src/stylesheet/AtRule.cpp
        src/stylesheet/CssComment.cpp
        src/stylesheet/Declaration.cpp
        src/stylesheet/MediaQuery.cpp
        src/stylesheet/Ruleset.cpp
        src/stylesheet/RulesetStatement.cpp
        src/stylesheet/Selector.cpp
        src/stylesheet/Stylesheet.cpp
        src/stylesheet/StylesheetStatement.cpp
        src/value/BooleanValue.cpp
        src/value/Color.cpp
        src/value/FunctionLibrary.cpp
        src/value/NumberValue.cpp
        src/value/StringValue.cpp
        src/value/UnitValue.cpp
        src/value/UrlValue.cpp
        src/value/Value.cpp
        src/value/ValueProcessor.cpp
        src/Token.cpp
        src/TokenList.cpp
        src/VariableMap.cpp
        )

add_library(lessc ${lessc_SOURCES})
target_include_directories(lessc PRIVATE src .)

add_executable(clessc src/lessc.cpp)
target_include_directories(clessc PRIVATE src .)
target_link_libraries(clessc lessc)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(GLOG)
if (GLOG_FOUND)
    target_compile_definitions(lessc PUBLIC WITH_LIBGLOG)
    target_include_directories(lessc PUBLIC ${GLOG_INCLUDE_DIRS})
    target_link_libraries(lessc ${GLOG_LIBRARIES})
    message(STATUS "Compile with libglog")
else (PNG_FOUND)
    message(WARNING "Did not find libglog")
endif (GLOG_FOUND)

find_package(PNG 1.2)
if (PNG_FOUND)
    target_compile_definitions(lessc PUBLIC WITH_LIBPNG)
    target_include_directories(lessc PUBLIC ${PNG_INCLUDE_DIRS})
    target_link_libraries(lessc ${PNG_LIBRARIES})
    message(STATUS "Compile with libpng")
else (PNG_FOUND)
    message(WARNING "Did not find libpng")
endif (PNG_FOUND)

find_package(JPEG)
if (JPEG_FOUND)
    target_compile_definitions(lessc PUBLIC WITH_LIBJPEG)
    target_include_directories(lessc PUBLIC ${JPEG_INCLUDE_DIRS})
    target_link_libraries(lessc ${JPEG_LIBRARIES})
    message(STATUS "Compile with libjpeg")
else (JPEG_FOUND)
    message(WARNING "Did not find libjpeg")
endif (JPEG_FOUND)

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
            tests/LessParser_test.cpp
            tests/ValueProcessor_test.cpp
            )

    add_executable(testlessc ${testlessc_SOURCES})
    target_include_directories(testlessc PRIVATE src ${GTEST_INCLUDE_DIRS})
    target_link_libraries(testlessc lessc ${GTEST_BOTH_LIBRARIES} Threads::Threads)
    add_test(lesscTest testlessc)
else (GTest_FOUND)
    message(WARNING "Did not find libgtest. Unable to test.")
endif (GTest_FOUND)
