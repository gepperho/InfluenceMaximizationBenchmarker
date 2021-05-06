include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DBUILD_STATIC_LIBS=ON
  -DBUILD_SHARED_LIBS=OFF
  -DFMT_TEST:BOOL=OFF
  -DFMT_INSTALL:BOOL=ON
  -DFMT_DOC:BOOL=OFF)

ExternalProject_Add(fmt-project
  PREFIX deps/fmt
  DOWNLOAD_NAME fmt-6.2.0.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/fmtlib/fmt/archive/6.2.0.tar.gz
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(fmt-project INSTALL_DIR)
add_library(fmt STATIC IMPORTED)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(FMT_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}fmtd${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
  set(FMT_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}fmt${CMAKE_STATIC_LIBRARY_SUFFIX})
endif ()

set(FMT_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${FMT_INCLUDE_DIR})  # Must exist.
set_property(TARGET fmt PROPERTY IMPORTED_LOCATION ${FMT_LIBRARY})
set_property(TARGET fmt PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${FMT_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
