include_directories (${PROJECT_SOURCE_DIR}/src/streamer/include)

add_executable (cortica-test cortica-test.cpp)
target_link_libraries (cortica-test cortica)

add_executable (cortica-opencv-test cortica-opencv-test.cpp)
target_link_libraries (cortica-opencv-test ${OpenCV_LIBS} cortica ${CMAKE_THREAD_LIBS_INIT})

add_executable (cortica-v4l2-embedded-test cortica-v4l2-embedded-test.cpp)
target_link_libraries (cortica-v4l2-embedded-test cortica)

add_executable (cortica-v4l2-test cortica-v4l2-test.cpp)
target_link_libraries (cortica-v4l2-test cortica v4l2uvc ${CMAKE_THREAD_LIBS_INIT})

message (INFO ${PROJECT_SOURCE_DIR})

set (PLUGIN_LIB
    ${PROJECT_SOURCE_DIR}/build/src/streamer/plugins/input_file/libinput_file.so
    ${PROJECT_SOURCE_DIR}/build/src/streamer/plugins/output_http/liboutput_http.so
)

file(COPY ${PLUGIN_LIB} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    @ONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

configure_files (${PROJECT_SOURCE_DIR}/examples/mjpg-streamer-www, ${CMAKE_CURRENT_BINARY_DIR})
