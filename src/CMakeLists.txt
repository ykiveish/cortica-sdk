include_directories (${PROJECT_SOURCE_DIR}/include)
include_directories (${PROJECT_SOURCE_DIR}/src/streamer/include)

add_subdirectory (video/v4l2uvc)
add_subdirectory (streamer)

add_library (cortica SHARED Cortica.cpp CorticaProviderCloud.cpp CorticaProviderFactory.cpp Utilities.cpp jsoncpp.cpp Streamer.cpp)
target_link_libraries (cortica curl v4l2uvc streamer ${CMAKE_THREAD_LIBS_INIT})