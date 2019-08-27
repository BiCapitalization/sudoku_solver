FetchContent_Declare(
    fmt
    GIT_REPOSITORY git@github.com:fmtlib/fmt.git
    GIT_TAG master
) 

FetchContent_Populate(fmt)
add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
