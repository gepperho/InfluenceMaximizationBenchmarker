include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=Release
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DBUILD_STATIC_LIBS=ON
  -DBUILD_SHARED_LIBS=OFF
  -Dgtest_build_samples=OFF
  -Dgtest_build_tests=OFF
  -DINSTALL_GTEST=ON
  -DBUILD_GMOCK=OFF)

ExternalProject_Add(gtest-project
  PREFIX deps/gtest
  DOWNLOAD_NAME gtest-1.8.1.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(gtest-project INSTALL_DIR)
add_library(gtest STATIC IMPORTED)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(GTEST_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtestd${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
  set(GTEST_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})
endif ()
set(GTEST_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${GTEST_INCLUDE_DIR})  # Must exist.
set_property(TARGET gtest PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARY})
set_property(TARGET gtest PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
