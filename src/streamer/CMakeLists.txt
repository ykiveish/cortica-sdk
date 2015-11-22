include_directories (${PROJECT_SOURCE_DIR}/src/streamer/include)

add_subdirectory (plugins)

add_library (streamer SHARED mjpg_streamer.c utils.c)
target_link_libraries (streamer dl ${CMAKE_THREAD_LIBS_INIT})
