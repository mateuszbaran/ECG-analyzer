project(ecg-analyzer)
cmake_minimum_required(VERSION 2.6)

INCLUDE(CheckLibraryExists)

# Additional CMake modules for 3rd party library checks reside here
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_admin")

find_package(Boost 1.48 REQUIRED)
find_package(Qt4 4.8 REQUIRED)
find_package(Qwt 6 REQUIRED)
find_package(gsl REQUIRED)
find_package(fftw 3.3 REQUIRED)

#check if some platform-specific libraries are needed for linking client and server
CHECK_LIBRARY_EXISTS(rt shm_open "" HAVE_RT_LIB)
if(HAVE_RT_LIB)
    set(RT_LIB -lrt)
endif()

CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_DL_LIB)
if(HAVE_DL_LIB)
    set(DL_LIB -ldl)
endif()

if(CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANGXX ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -Wpointer-arith -Wno-switch -Wno-sign-compare -Wno-unused-parameter -Wno-overloaded-virtual -fpermissive")
endif()

FILE(GLOB ecg-analyzer_HEADERS *.h *.hpp)
FILE(GLOB ecg-analyzer_SOURCES *.cpp)
FILE(GLOB plots_HEADERS plots/*.h)
FILE(GLOB plots_SOURCES plots/*.cpp)

SET(ecg-analyzer_FORMS ecganalyzer.ui)
SET(ecg-analyzer_AboutWindow_FORMS AboutWindow.ui)

SET(ecg-analyzer_RESOURCES ecganalyzer.qrc)

QT4_WRAP_CPP(ecg-analyzer_HEADERS_MOC ${ecg-analyzer_HEADERS} ${plots_HEADERS})
QT4_WRAP_UI(ecg-analyzer_FORMS_HEADERS ${ecg-analyzer_FORMS})
QT4_WRAP_UI(ecg-analyzer_AboutWindow_FORMS_HEADERS ${ecg-analyzer_AboutWindow_FORMS})
QT4_ADD_RESOURCES(ecg-analyzer_RESOURCES_RCC ${ecg-analyzer_RESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# Controls behaviour of logger - possible values: -DCLEANTLOG, -DFTLOG, -DETLOG, -DTLOG
ADD_DEFINITIONS("-DCLEANTLOG")

include_directories(${GSL_INCLUDE_DIRS} ${GSLCBLAS_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GSL_LIBRARIES} ${GSLCBLAS_LIBRARIES})

add_subdirectory(wfdb)

include_directories(${CMAKE_HOME_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QWT_INCLUDE_DIR})
include_directories(plots)

link_libraries(${LIBS})

ADD_EXECUTABLE(ecg-analyzer ${ecg-analyzer_SOURCES} ${plots_SOURCES}
    ${ecg-analyzer_HEADERS_MOC} ${ecg-analyzer_FORMS_HEADERS} ${ecg-analyzer_AboutWindow_FORMS_HEADERS} ${ecg-analyzer_RESOURCES_RCC} )

TARGET_LINK_LIBRARIES(ecg-analyzer ${QT_LIBRARIES} ${QWT_LIBRARY} ${LIBS} ${FFTW_LIBRARIES} wfdb)
