add_library(slipstream_marketdata STATIC
    l1_book.cpp
    rolling_vwap.cpp
)
target_include_directories(slipstream_marketdata PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_marketdata PUBLIC slipstream_core slipstream_codec)
