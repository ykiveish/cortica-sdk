include_directories (${PROJECT_SOURCE_DIR}/include)

add_library (v4l2uvc SHARED v4l2uvc.c)
target_link_libraries (v4l2uvc)