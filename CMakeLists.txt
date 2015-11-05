cmake_minimum_required (VERSION 2.8)
project (cortica-sdk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

find_package( OpenCV REQUIRED )
message (INFO "Cortica configuration.")

include_directories (
	${PROJECT_SOURCE_DIR}/include
)

link_directories (
	${PROJECT_SOURCE_DIR}/libs
)

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_subdirectory (src)

add_executable (cortica-test cortica-test.cpp)
target_link_libraries (cortica-test cortica)

add_executable (cortica-opencv-test cortica-opencv-test.cpp)
target_link_libraries (cortica-opencv-test ${OpenCV_LIBS} cortica)