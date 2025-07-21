cmake_minimum_required(VERSION 3.15)
project(DefectStudioCXX VERSION 1.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Gather source and header files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)

# Build the executable
add_executable(DefectStudioCXX ${SOURCES} ${HEADERS})
target_include_directories(DefectStudioCXX PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Installation: executable and headers
install(TARGETS DefectStudioCXX
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# Optional: create a basic Config file (not necessary unless you're exporting as a tool dependency)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DefectStudioCXXConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/DefectStudioCXXConfig.cmake
    INSTALL_DESTINATION lib/cmake/DefectStudioCXX
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/DefectStudioCXXConfig.cmake
    DESTINATION lib/cmake/DefectStudioCXX
)
