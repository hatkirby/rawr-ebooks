cmake_minimum_required (VERSION 3.1)
project (rawr-ebooks)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vendor/libtwittercpp)
include_directories(vendor/libtwittercpp/src vendor/libtwittercpp/vendor/json/src vendor/libtwittercpp/vendor/curlcpp/include)

add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)
include_directories(vendor/yaml-cpp/include)

find_package(ASPELL REQUIRED)
include_directories(${ASPELL_INCLUDE_DIR})

add_library(rawr kgramstats.cpp histogram.cpp prefix_search.cpp)
set_property(TARGET rawr PROPERTY CXX_STANDARD 11)
set_property(TARGET rawr PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr ${ASPELL_LIBRARIES})

add_executable(rawr-ebooks ebooks.cpp)
set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD 11)
set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr-ebooks rawr yaml-cpp twitter++ curlcpp curl pthread)

add_executable(rawr-gen gen.cpp)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD 11)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr-gen rawr)
