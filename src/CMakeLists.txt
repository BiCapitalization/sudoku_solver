add_library(ssolve STATIC input.cpp data.cpp solver.cpp toroidal_list.cpp)
add_executable(sudoku_solve main.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(ssolve PRIVATE ${GNU_CLANG_WARNING_FLAGS})
    target_compile_options(sudoku_solve PRIVATE ${GNU_CLANG_WARNING_FLAGS})
endif()

target_link_libraries(ssolve PUBLIC expected fmt::fmt)
target_link_libraries(sudoku_solve PRIVATE ssolve)
