include_directories (${PROJECT_SOURCE_DIR}/include)

add_library (cortica SHARED Cortica.cpp CorticaProviderCloud.cpp CorticaProviderFactory.cpp jsoncpp.cpp)
target_link_libraries (cortica curl)