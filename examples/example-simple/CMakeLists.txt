project(example-simple LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(example-simple main.cc)
target_link_libraries(example-simple PRIVATE Qt6::Core qconsole)
