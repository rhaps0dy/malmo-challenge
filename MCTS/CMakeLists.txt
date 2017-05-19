cmake_minimum_required(VERSION 2.8.11)
project(SickleblowOne)

function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

set(CONCAT_CXX_FILES node.cpp uct.cpp strategy.cpp path_cache.cpp)

#CXXFLAGS
set(CMAKE_CXX_FLAGS_DEBUG  "-g -D_GLIBCXX_DEBUG -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -D_GLIBCXX_DEBUG")
add_definitions(-std=c++14)

set(CMAKE_MACOSX_RPATH 1)

add_executable(main ${CONCAT_CXX_FILES})
add_library(uct SHARED ${CONCAT_CXX_FILES})

