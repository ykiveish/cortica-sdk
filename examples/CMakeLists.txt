include_directories (${PROJECT_SOURCE_DIR}/src/streamer/include)

add_executable (cortica-test cortica-test.cpp)
target_link_libraries (cortica-test cortica)

add_executable (cortica-opencv-test cortica-opencv-test.cpp)
target_link_libraries (cortica-opencv-test ${OpenCV_LIBS} cortica ${CMAKE_THREAD_LIBS_INIT})

add_executable (cortica-v4l2-embedded-test cortica-v4l2-embedded-test.cpp)
target_link_libraries (cortica-v4l2-embedded-test ${OpenCV_LIBS} cortica)

add_executable (cortica-v4l2-test cortica-v4l2-test.cpp)
target_link_libraries (cortica-v4l2-test cortica v4l2uvc ${CMAKE_THREAD_LIBS_INIT})

message (INFO ${PROJECT_SOURCE_DIR})
