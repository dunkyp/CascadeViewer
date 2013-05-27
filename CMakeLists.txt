cmake_minimum_required(VERSION 2.8.11)

find_package(OCE)
find_package(Qt4)
find_package(VTK)
set(Boost_USE_MULTITHREADED ON)
find_package(boost)

set(CMAKE_AUTOMOC ON)

if(OCE_FOUND)
	message(STATUS "Found OCE v. ${OCE_VERSION}")
	include_directories(${OCE_INCLUDE_DIRS})
endif(OCE_FOUND)

if(Boost_Found)
	message(STATUS "Found Boost")
	include_directories($(Boost_INCLUDE_DIRS))
endif()
INCLUDE(${QT_USE_FILE})
INCLUDE(${VTK_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


add_executable(test main.cpp steploader.cpp displaywindow.cpp triangulate.cpp vtk.cpp stlloader.cpp graph.cpp)
target_link_libraries(test TKPrim TKSTEP TKXDESTEP ${QT_LIBRARIES} ${VTK_LIBRARIES})