include_directories (${PROJECT_SOURCE_DIR}/src/streamer/include)

add_library (input_file SHARED input_file.c)
target_link_libraries (input_file)