FetchContent_Declare(
    catch2
    GIT_REPOSITORY git@github.com:catchorg/Catch2.git
    GIT_TAG v2.9.2
)

FetchContent_Populate(catch2)
add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
