project(alglib)
cmake_minimum_required(VERSION 2.6)

# set(alglib_SOURCES ${alglib_SOURCES} *.cpp  )
# set(alglib_HEADERS ${alglib_HEADERS} *.h  )
FILE(GLOB alglib_HEADERS *.h)
FILE(GLOB alglib_SOURCES *.cpp)


add_library(alglib ${alglib_SOURCES})
