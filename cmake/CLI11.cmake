include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=Release
  -DCLI11_TESTING=OFF
  -DCLI11_SINGLE_FILE=OFF
  -DCLI11_EXAMPLES=OFF)

ExternalProject_Add(CLI11-project
  PREFIX deps/CLI11
  DOWNLOAD_NAME CLI11-1.8.0.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/CLIUtils/CLI11/archive/v1.8.0.tar.gz
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )

ExternalProject_Get_Property(CLI11-project INSTALL_DIR)
set(CLI11_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
