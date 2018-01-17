cmake_minimum_required (VERSION 3.1)
project (rawr-ebooks)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vendor/libtwittercpp)

find_package(ASPELL REQUIRED)

find_package(PkgConfig)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)

include_directories(
  vendor/libtwittercpp/src
  ${ASPELL_INCLUDE_DIR}
  ${yaml-cpp_INCLUDE_DIRS})

add_library(rawr kgramstats.cpp histogram.cpp prefix_search.cpp)
set_property(TARGET rawr PROPERTY CXX_STANDARD 11)
set_property(TARGET rawr PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr ${ASPELL_LIBRARIES})

add_executable(rawr-ebooks ebooks.cpp)
set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD 11)
set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr-ebooks rawr ${yaml-cpp_LIBRARIES} twitter++)

add_executable(rawr-gen gen.cpp)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD 11)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr-gen rawr)
