add_library(slipstream_net STATIC
    socket_fd.cpp
)
target_include_directories(slipstream_net PUBLIC ${PROJECT_SOURCE_DIR}/include)
