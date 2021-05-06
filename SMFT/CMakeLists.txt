add_library(SFMT ${CMAKE_CURRENT_SOURCE_DIR}/src/dSFMT.c)

# Make sure the compiler can find include files for 
# when it links to it
target_include_directories (SFMT PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(SFMTTest ${CMAKE_CURRENT_SOURCE_DIR}/test/test.c)
target_link_libraries(SFMTTest SFMT)

add_test(SFMTTest SFMTTest)
