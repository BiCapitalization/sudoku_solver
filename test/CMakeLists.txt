add_executable(test test_main.cpp data_test.cpp solver_test.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(test PRIVATE ${GNU_CLANG_WARNING_FLAGS})
endif()

target_include_directories(test PRIVATE ${ADDITIONAL_INCLUDE_DIRS})
target_link_libraries(test PRIVATE ssolve Catch2::Catch2)

set_target_properties(test PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
