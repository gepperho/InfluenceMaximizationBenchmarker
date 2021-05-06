cmake_minimum_required(VERSION 3.14)
#get options and compiling env
include(cmake/options.cmake)
include(cmake/linker.cmake)
include(cmake/ccache.cmake)

project(InfluenceMaximizationBenchmarker)
project(InfluenceMaximizationBenchmarker LANGUAGES CXX)

#set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

############################
###get external projects
############################
include(cmake/fmt.cmake)
include(cmake/tbb.cmake)
include(cmake/span.cmake)
include(cmake/CLI11.cmake)
include(cmake/gtest.cmake)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


include(cmake/flags.cmake)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(SMFT)


# needed for multithreading
find_package(Threads REQUIRED)


#############################################
## library with all the logic which
## can later be easily linked against test
#############################################
add_library(InfluenceMaximizationBenchmarkerSrc "")

target_sources(InfluenceMaximizationBenchmarkerSrc
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include/Graph.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/diffusion/IndependentCascade.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/Edge.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/Parser.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/Timer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/IMBProgramOptions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/diffusion/LinearThreshold.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/diffusion/DiffusionModelInterface.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/diffusion/DiffusionFactory.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/util/Definitions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/SolverInterface.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/SolverFactory.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/simple/DegreeDiscountSolver.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/simple/PageRankSolver.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/simple/WeightedPageRank.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/simple/HighDegree.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/simple/Random.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/iterative/EaSyIM_Delta.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/iterative/UpdateApproximation.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/iterative/EaSyIM.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/WeightedDegree.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/IPA.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/IMM.hpp
        ${CMAKE_CURRENT_LIST_DIR}/include/solver/CelfGreedy.hpp

        PRIVATE
        src/IMBProgramOptions.cpp
        src/Graph.cpp
        src/diffusion/IndependentCascade.cpp
        src/Edge.cpp
        src/Parser.cpp
        src/solver/simple/DegreeDiscountSolver.cpp
        src/solver/simple/PageRankSolver.cpp
        src/solver/WeightedDegree.cpp
        src/solver/IPA.cpp
        src/solver/iterative/EaSyIM_Delta.cpp
        src/solver/simple/WeightedPageRank.cpp
        src/solver/IMM.cpp
        src/diffusion/LinearThreshold.cpp
        src/solver/simple/HighDegree.cpp
        src/solver/simple/Random.cpp
        src/solver/CelfGreedy.cpp
        src/solver/iterative/UpdateApproximation.cpp
        src/solver/SolverFactory.cpp
        src/solver/iterative/EaSyIM.cpp
        )

# add the dependencies of the target to enforce
# the right order of compiling
add_dependencies(InfluenceMaximizationBenchmarkerSrc fmt-project)
add_dependencies(InfluenceMaximizationBenchmarkerSrc tbb-project)
add_dependencies(InfluenceMaximizationBenchmarkerSrc span-lite-project)
add_dependencies(InfluenceMaximizationBenchmarkerSrc CLI11-project)

# make headers available
target_include_directories(InfluenceMaximizationBenchmarkerSrc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        fmt
        tbb
        ${CLI11_INCLUDE_DIR}
        ${SPAN_LITE_INCLUDE_DIR}
        )

#link against libarys
target_link_libraries(InfluenceMaximizationBenchmarkerSrc LINK_PUBLIC
        fmt
        SFMT
        tbb)

target_link_libraries(InfluenceMaximizationBenchmarkerSrc PRIVATE
        ${CMAKE_THREAD_LIBS_INIT})


###############################
## THE ACTUAL BINARY
###############################
add_executable(InfluenceMaximizationBenchmarker main.cpp)

# make headers available
target_include_directories(InfluenceMaximizationBenchmarker PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        fmt
        tbb
        ${CLI11_INCLUDE_DIR}
        ${SPAN_LITE_INCLUDE_DIR}
        )

#link against libarys
target_link_libraries(InfluenceMaximizationBenchmarker LINK_PUBLIC
        InfluenceMaximizationBenchmarkerSrc
        fmt
        tbb
        SFMT
        ${CMAKE_THREAD_LIBS_INIT})

# add the dependencies of the target to enforce
# the right order of compiling
add_dependencies(InfluenceMaximizationBenchmarker InfluenceMaximizationBenchmarkerSrc)


###############################
## THE EVALUATOR BINARY
###############################
add_executable(InfluenceMaximizationEvaluator evaluator.cpp)

# make headers available
target_include_directories(InfluenceMaximizationEvaluator PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        fmt
        tbb
        ${CLI11_INCLUDE_DIR}
        ${SPAN_LITE_INCLUDE_DIR}
        )

#link against libarys
target_link_libraries(InfluenceMaximizationEvaluator LINK_PUBLIC
        InfluenceMaximizationBenchmarkerSrc
        fmt
        tbb
        SFMT
        ${CMAKE_THREAD_LIBS_INIT})

# add the dependencies of the target to enforce
# the right order of compiling
add_dependencies(InfluenceMaximizationEvaluator InfluenceMaximizationBenchmarkerSrc)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif (BUILD_TESTS)
