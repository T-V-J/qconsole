project(test-qconsole LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Test)
set(CMAKE_AUTOMOC ON)

add_executable(test-qconsole "test-qconsole.h" "test-qconsole.cc")

target_include_directories(test-qconsole PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test-qconsole PRIVATE Qt6::Test qconsole)

add_test(NAME test-qconsole COMMAND test-qconsole)
