cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(less_SOURCES
        src/css/CssParser.cpp
        src/css/CssPrettyWriter.cpp
        src/css/CssTokenizer.cpp
        src/css/CssWriter.cpp
        src/css/ParseException.cpp
        src/css/SourceMapWriter.cpp
        src/css/CssSelectorParser.cpp
        src/less/LessParser.cpp
        src/less/LessTokenizer.cpp
        src/less/LessSelectorParser.cpp
        src/lessstylesheet/Closure.cpp
        src/lessstylesheet/Extension.cpp
        src/lessstylesheet/LessAtRule.cpp
        src/lessstylesheet/LessMediaQuery.cpp
        src/lessstylesheet/LessRuleset.cpp
        src/lessstylesheet/LessDeclaration.cpp
        src/lessstylesheet/LessSelector.cpp
        src/lessstylesheet/LessStylesheet.cpp
        src/lessstylesheet/MediaQueryRuleset.cpp
        src/lessstylesheet/Mixin.cpp
        src/lessstylesheet/MixinArguments.cpp
        src/lessstylesheet/MixinCall.cpp
        src/lessstylesheet/MixinException.cpp
        src/lessstylesheet/ProcessingContext.cpp
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
        src/value/ColorFunctions.cpp
        src/value/NumberFunctions.cpp
        src/value/StringFunctions.cpp
        src/value/UrlFunctions.cpp
        src/Token.cpp
        src/TokenList.cpp
        src/VariableMap.cpp
        src/LessException.cpp
        )

add_library(less SHARED ${less_SOURCES})
target_include_directories(less PUBLIC include)

install(TARGETS less LIBRARY DESTINATION lib)
install(DIRECTORY include/less DESTINATION include)

find_package(PNG 1.2)
if (PNG_FOUND)
    target_compile_definitions(less PUBLIC WITH_LIBPNG)
    target_include_directories(less PUBLIC ${PNG_INCLUDE_DIRS})
    target_link_libraries(less ${PNG_LIBRARIES})
    message(STATUS "Compile with libpng")
else (PNG_FOUND)
    message(WARNING "Did not find libpng")
endif (PNG_FOUND)

find_package(JPEG)
if (JPEG_FOUND)
    target_compile_definitions(less PUBLIC WITH_LIBJPEG)
    target_include_directories(less PUBLIC ${JPEG_INCLUDE_DIRS})
    target_link_libraries(less ${JPEG_LIBRARIES})
    message(STATUS "Compile with libjpeg")
else (JPEG_FOUND)
    message(WARNING "Did not find libjpeg")
endif (JPEG_FOUND)
