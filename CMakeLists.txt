cmake_minimum_required(VERSION 3.14)

project(qconsole VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(QCONSOLE_BUILD_EXAMPLES "Build the examples '/examples/'" OFF)
option(QCONSOLE_BUILD_TESTS "Build the tests '/tests/'" OFF)

add_compile_definitions(QT_NO_KEYWORDS
                        QT_NO_JAVA_STYLE_ITERATORS
                        QT_USE_QSTRINGBUILDER
                        QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
                        QT_NO_FOREACH
)

add_subdirectory(src)

if(QCONSOLE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(QCONSOLE_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
