add_library(slipstream_metrics STATIC
    latency_recorder.cpp
)
target_include_directories(slipstream_metrics PUBLIC ${PROJECT_SOURCE_DIR}/include)
