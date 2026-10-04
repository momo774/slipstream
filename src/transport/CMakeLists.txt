add_library(slipstream_transport STATIC
    tcp/tcp_feed_transport.cpp
)
target_include_directories(slipstream_transport PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_transport PUBLIC slipstream_net slipstream_codec)

