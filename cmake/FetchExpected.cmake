FetchContent_Declare(
    expected
    GIT_REPOSITORY git@github.com:TartanLlama/expected.git
    GIT_TAG master
)

FetchContent_Populate(expected)
add_subdirectory(${expected_SOURCE_DIR} ${expected_BINARY_DIR})
