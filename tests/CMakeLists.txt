include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(slipstream_tests
    codec/frame_codec_test.cpp
    codec/md_codec_test.cpp
    codec/oe_codec_test.cpp
    codec/stream_reassembly_test.cpp
    codec/byte_order_test.cpp
)
target_include_directories(slipstream_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(slipstream_tests PRIVATE
    GTest::gtest_main
    slipstream_codec
)

include(GoogleTest)
gtest_discover_tests(slipstream_tests)
