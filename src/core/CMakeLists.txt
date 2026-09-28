add_library(slipstream_core STATIC
    fixed_point.cpp
)
target_include_directories(slipstream_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
