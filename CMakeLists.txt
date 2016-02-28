cmake_minimum_required (VERSION 2.6)
project (rawr-ebooks)

find_package(PkgConfig)
pkg_check_modules(Curl libcurl)

if (Curl_FOUND)
  add_subdirectory(vendor/twitcurl/libtwitcurl)
  include_directories(vendor/twitcurl/libtwitcurl)
  
  add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)
  include_directories(vendor/yaml-cpp/include)
  
  add_executable(rawr-ebooks ebooks.cpp kgramstats.cpp freevars.cpp histogram.cpp prefix_search.cpp)
  set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD 11)
  set_property(TARGET rawr-ebooks PROPERTY CXX_STANDARD_REQUIRED ON)
  target_link_libraries(rawr-ebooks yaml-cpp twitcurl ${Curl_LIBRARIES} aspell)
else (Curl_FOUND)
  message(STATUS "rawr-ebooks requires yaml-cpp and twitcurl; without these, we will only make rawr-gen")
endif (Curl_FOUND)

add_executable(rawr-gen gen.cpp kgramstats.cpp freevars.cpp histogram.cpp prefix_search.cpp)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD 11)
set_property(TARGET rawr-gen PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(rawr-gen aspell)
