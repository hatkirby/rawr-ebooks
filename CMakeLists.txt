cmake_minimum_required (VERSION 2.6)
project (rawr-ebooks)

add_subdirectory(vendor/twitcurl/libtwitcurl)

find_package(PkgConfig)
pkg_check_modules(YamlCpp yaml-cpp REQUIRED)

include_directories(vendor/twitcurl/libtwitcurl)
add_executable(rawr-ebooks ebooks.cpp kgramstats.cpp freevars.cpp malaprop.cpp)
target_link_libraries(rawr-ebooks ${YamlCpp_LIBRARIES} twitcurl curl)

add_executable(rawr-gen gen.cpp kgramstats.cpp freevars.cpp malaprop.cpp)
