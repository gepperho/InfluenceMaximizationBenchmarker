include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=Release
        -DSPAN_LITE_OPT_BUILD_TESTS=OFF
        -DSPAN_LITE_OPT_BUILD_EXAMPLES=OFF
        -DSPAN_LITE_EXPORT_PACKAGE=OFF
        -DSPAN_LITE_COLOURISE_TEST=OFF)

ExternalProject_Add(span-lite-project
        PREFIX deps/span-lite
        DOWNLOAD_NAME span-lite-0.10.3.tar.gz
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
        URL https://github.com/martinmoene/span-lite/archive/v0.10.3.tar.gz
        CMAKE_ARGS ${CMAKE_ARGS}
        # Overwrite build and install commands to force Release build on MSVC.
        BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
        INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )

ExternalProject_Get_Property(span-lite-project INSTALL_DIR)
set(SPAN_LITE_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
