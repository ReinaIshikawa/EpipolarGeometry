cmake_minimum_required(VERSION 3.13)

#Create Project
project( EpipolarGeometry )
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH} )

add_executable(epipolar epipolar.cpp)
add_executable(mosaic mosaic.cpp)


# Set StartUp Project
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "epipolar")
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "mosaic")

set(CMAKE_CXX_STANDARD 14)

#FindPackage
find_package( OpenCV REQUIRED )

if(OpenCV_FOUND)
  include_directories( ${OpenCV_INCLUDE_DIRS})
  target_link_libraries( epipolar ${OpenCV_LIBS})
  target_link_libraries( mosaic ${OpenCV_LIBS})
endif(OpenCV_FOUND)