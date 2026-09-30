add_library(slipstream_codec STATIC
    encoder.cpp
    byte_order.cpp
)
target_include_directories(slipstream_codec PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_codec PUBLIC slipstream_core)
