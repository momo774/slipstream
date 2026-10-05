add_library(slipstream_strategy STATIC
    decision_engine.cpp
)
target_include_directories(slipstream_strategy PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_strategy PUBLIC slipstream_marketdata slipstream_core)
