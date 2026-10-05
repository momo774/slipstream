add_library(slipstream_cli STATIC
    options.cpp
)
target_include_directories(slipstream_cli PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_cli PUBLIC slipstream_core)
