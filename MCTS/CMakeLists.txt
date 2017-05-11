cmake_minimum_required(VERSION 2.8.11)
project(SickleblowOne)

function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

set(CONCAT_CXX_FILES node.cpp uct.cpp strategy.cpp)

#file(WRITE _all.cpp "")
#foreach(CONCAT_CXX_FILE ${CONCAT_CXX_FILES})
#  cat(${CONCAT_CXX_FILE} _all.cpp)
#endforeach()

#CXXFLAGS
set(CMAKE_CXX_FLAGS_DEBUG  "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DRELEASE")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
add_definitions(-std=c++14 -D_GLIBCXX_DEBUG)

set(CMAKE_MACOSX_RPATH 1)

#add_executable(main _all.cpp)
#add_executable(main ${CONCAT_CXX_FILES})
add_library(mcts SHARED ${CONCAT_CXX_FILES})
