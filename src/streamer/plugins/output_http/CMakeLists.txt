include_directories (${PROJECT_SOURCE_DIR}/src/streamer/include)

add_library (output_http SHARED output_http.c httpd.c)
target_link_libraries (output_http ${CMAKE_THREAD_LIBS_INIT})