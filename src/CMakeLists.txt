include(FetchContent)
include(GNUInstallDirs)

fetchcontent_declare(replxx
                     GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx/
                     GIT_TAG e598577ea899630ac8f24e4ab228d04e8abd4d35
)

fetchcontent_declare(hattrie
                     GIT_REPOSITORY https://github.com/Tessil/hat-trie.git
                     GIT_TAG d0d3ff6e1ec6c835cac9b510ae8f9375babb8004
)

fetchcontent_makeavailable(replxx hattrie)

find_package(Qt6 REQUIRED COMPONENTS Core)
set(CMAKE_AUTOMOC ON)

add_library(qconsole STATIC "qconsole.cc")

target_link_libraries(qconsole PRIVATE Qt6::Core replxx::replxx)

target_include_directories(qconsole
                           PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                           PRIVATE "$<BUILD_INTERFACE:${hattrie_SOURCE_DIR}/include>"
)

install(TARGETS qconsole
        EXPORT qconsole-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES "qconsole.h" "QConsole" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)

configure_package_config_file("${CMAKE_SOURCE_DIR}/cmake/qconsole.cmake.in"
                              "${CMAKE_SOURCE_DIR}/cmake/qconsole.cmake"
                              INSTALL_DESTINATION "lib/cmake/qconsole"
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO
                              NO_SET_AND_CHECK_MACRO
)

write_basic_package_version_file("${CMAKE_SOURCE_DIR}/cmake/qconsole-version.cmake"
                                 COMPATIBILITY AnyNewerVersion
)

install(EXPORT qconsole-targets DESTINATION "lib/cmake/qconsole")

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE"
        DESTINATION "share/licenses/qconsole"
)

install(FILES "${replxx_SOURCE_DIR}/LICENSE.md"
        DESTINATION "share/licenses/replxx"
)

install(FILES "${hattrie_SOURCE_DIR}/LICENSE"
        DESTINATION "share/licenses/hat-trie"
)

install(FILES "${CMAKE_SOURCE_DIR}/cmake/qconsole-version.cmake"
              "${CMAKE_SOURCE_DIR}/cmake/qconsole.cmake"
        DESTINATION "lib/cmake/qconsole"
)
