add_library(slipstream_session STATIC
    heartbeat_monitor.cpp
)
target_include_directories(slipstream_session PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_session PUBLIC slipstream_codec slipstream_transport)
