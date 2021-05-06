# use ld.gold linker if available, could make optional
execute_process(
  COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version
  ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
if ("${LD_VERSION}" MATCHES "GNU gold")
  message(STATUS "found GNU gold linker, using it as linker")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
else ()
  message(STATUS "GNU gold linker isn't available, using the default system linker.")
endif ()
unset(LD_VERSION)
