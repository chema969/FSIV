CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(cv_tutorial)
FIND_PACKAGE(OpenCV 	REQUIRED )
include_directories ("${OpenCV_INCLUDE_DIRS}")
#Required for uco thinstation. May not be required in your own pc
SET(CMAKE_CXX_FLAGS " -D_GLIBCXX_USE_CXX11_ABI=0 -m32")
LINK_LIBRARIES(${OpenCV_LIBS} )
add_executable(prg1 prg1.cpp)
add_executable(prg2 prg2.cpp)
add_executable(prg3 prg3.cpp)

 
