project(example-complex LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(example-complex main.cc)
target_link_libraries(example-complex PRIVATE Qt6::Core Qt6::Network qconsole)
