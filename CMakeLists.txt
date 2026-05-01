cmake_minimum_required(VERSION 3.10)
project(rmSafe VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable
add_executable(rmSafe main.cpp)

if(WIN32)
    target_link_libraries(rmSafe shell32)
    # Static link runtime to create standalone executable on Windows
    set_property(TARGET rmSafe PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

# Installation rules
install(TARGETS rmSafe RUNTIME DESTINATION bin)

# CPack configuration
set(CPACK_PACKAGE_NAME "rmsafe")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Safe rm command that moves files to the recycle bin/trash instead of deleting permanently.")
set(CPACK_PACKAGE_VENDOR "rmSafe Project")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "user@example.com")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
elseif(WIN32)
    set(CPACK_GENERATOR "ZIP") # Produces a zip with the .exe inside
endif()

include(CPack)
