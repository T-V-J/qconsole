project(example-widgets LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(example-widgets main.cc)
target_link_libraries(example-widgets PRIVATE Qt6::Core Qt6::Widgets qconsole)
