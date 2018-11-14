CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(P3)
FIND_PACKAGE(OpenCV 	REQUIRED )
include_directories ("${OpenCV_INCLUDE_DIRS}")
LINK_LIBRARIES(${OpenCV_LIBS} )
add_executable(main Main.cpp auxiliarFunctions.cpp)
add_executable(calibracion calibracion.cpp)


 
