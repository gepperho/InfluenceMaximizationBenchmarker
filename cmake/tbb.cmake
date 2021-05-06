include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DBUILD_STATIC_LIBS=ON
  -DBUILD_SHARED_LIBS=OFF
  -DTBB_BUILD_STATIC=ON
  -DTBB_BUILD_SHARED=OFF
  -DTBB_CI_BUILD=OFF
  -DTBB_BUILD_TESTS=OFF)

ExternalProject_Add(tbb-project
  PREFIX deps/tbb
  # DOWNLOAD_NAME tbb-git
  # DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  GIT_REPOSITORY https://github.com/wjakob/tbb.git
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  UPDATE_COMMAND ""
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(tbb-project INSTALL_DIR)
add_library(tbb STATIC IMPORTED)
set(TBB_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}tbb_static${CMAKE_STATIC_LIBRARY_SUFFIX})
set(TBB_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${TBB_INCLUDE_DIR})  # Must exist.
set_property(TARGET tbb PROPERTY IMPORTED_LOCATION ${TBB_LIBRARY})
set_property(TARGET tbb PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${TBB_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
