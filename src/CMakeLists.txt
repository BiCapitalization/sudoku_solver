add_library(ssolve STATIC input.cpp data.cpp solver.cpp toroidal_list.cpp)
add_executable(sudoku_solve main.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        ${GNU_CLANG_WARNING_FLAGS}
        $<$<CONFIG:Release>:${GNU_CLANG_OPTIMIZATION_FLAGS}>)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT HAS_IPO)

if(HAS_IP)
    set_property(TARGET ssolve sudoku_solve 
        PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_link_libraries(ssolve PUBLIC expected fmt::fmt)
target_link_libraries(sudoku_solve PRIVATE ssolve)
