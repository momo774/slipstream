add_library(slipstream_report STATIC
    execution_report.cpp
)
target_include_directories(slipstream_report PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_report PUBLIC slipstream_core slipstream_marketdata)
