add_executable(slipstream
    main.cpp
    md_server.cpp
)
target_include_directories(slipstream PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream PRIVATE
    slipstream_core
    slipstream_net
    slipstream_codec
    slipstream_transport
    slipstream_marketdata
    slipstream_cli
)
