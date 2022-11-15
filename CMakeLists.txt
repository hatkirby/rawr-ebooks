cmake_minimum_required (VERSION 3.1)
project (rawr-ebooks)

find_package(ASPELL REQUIRED)

find_package(PkgConfig)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)
pkg_check_modules(mastodonpp mastodonpp REQUIRED)

include_directories(
  vendor/json
  vendor/hkutil
  ${mastodonpp_INCLUDE_DIRS}
  ${ASPELL_INCLUDE_DIR}
  ${yaml-cpp_INCLUDE_DIRS})

link_directories(
  ${mastodonpp_LIBRARY_DIRS}
  ${yaml-cpp_LIBRARY_DIRS})

add_library(rawr kgramstats.cpp prefix_search.cpp)
set_property(TARGET rawr PROPERTY CXX_STANDARD 17)
set_property(TARGET rawr PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr ${ASPELL_LIBRARIES})

add_executable(rawr-ebooks ebooks.cpp timeline.cpp)
set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD 17)
set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr-ebooks rawr ${yaml-cpp_LIBRARIES} ${mastodonpp_LIBRARIES})

add_executable(rawr-gen gen.cpp)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD 17)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr-gen rawr)
