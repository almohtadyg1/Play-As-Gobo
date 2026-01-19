# =============================================================================
# PlayAsGobo - Cross-Platform Game Project
# =============================================================================
cmake_minimum_required(VERSION 3.15...3.28)

# Project declaration with proper language handling
if(WIN32)
    project(PlayAsGobo 
        VERSION 1.0.0
        DESCRIPTION "A cross-platform Raylib game where you play as the enemy"
        LANGUAGES CXX RC
    )
else()
    project(PlayAsGobo 
        VERSION 1.0.0
        DESCRIPTION "A cross-platform Raylib game where you play as the enemy"
        LANGUAGES CXX
    )
endif()

# =============================================================================
# CMake Policies
# =============================================================================
if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW)
endif()

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# =============================================================================
# Global Configuration
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
        "Choose the type of build (Debug, Release, MinSizeRel, RelWithDebInfo)" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Enable folder organization in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# =============================================================================
# Platform Detection
# =============================================================================
set(PLATFORM_WINDOWS FALSE)
set(PLATFORM_LINUX FALSE)
set(PLATFORM_MACOS FALSE)

if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Target Platform: Windows")
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
    message(STATUS "Target Platform: macOS")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Target Platform: Linux")
endif()

# =============================================================================
# Compiler Configuration
# =============================================================================
set(COMPILER_MSVC FALSE)
set(COMPILER_GCC FALSE)
set(COMPILER_CLANG FALSE)

if(MSVC)
    set(COMPILER_MSVC TRUE)
    message(STATUS "Compiler: MSVC ${MSVC_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_GCC TRUE)
    message(STATUS "Compiler: GCC ${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_CLANG TRUE)
    message(STATUS "Compiler: Clang ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# =============================================================================
# Optional Features
# =============================================================================
option(ENABLE_ASAN "Enable AddressSanitizer (Debug builds only)" OFF)

# =============================================================================
# Dependencies
# =============================================================================
# Find Raylib
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib/CMakeLists.txt")
    message(STATUS "Using local Raylib from libs/raylib")
    
    # Save current warning flags
    set(OLD_CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(OLD_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    
    # Suppress all warnings for raylib
    if(COMPILER_MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
    endif()
    
    add_subdirectory(libs/raylib)
    
    # Restore warning flags for our project
    set(CMAKE_C_FLAGS "${OLD_CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${OLD_CMAKE_CXX_FLAGS}")
    
    # Extra protection: disable warnings directly on raylib target
    if(TARGET raylib)
        if(COMPILER_MSVC)
            target_compile_options(raylib PRIVATE /w)
        else()
            target_compile_options(raylib PRIVATE -w)
        endif()
        
        # Mark raylib headers as system headers to suppress warnings
        get_target_property(RAYLIB_INCLUDE_DIRS raylib INTERFACE_INCLUDE_DIRECTORIES)
        if(RAYLIB_INCLUDE_DIRS)
            set_target_properties(raylib PROPERTIES
                INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${RAYLIB_INCLUDE_DIRS}"
            )
        endif()
    endif()
    
    set(RAYLIB_TARGET raylib)
    message(STATUS "Raylib warnings suppressed")
else()
    find_package(raylib QUIET)
    if(raylib_FOUND)
        message(STATUS "Found system Raylib")
        set(RAYLIB_TARGET raylib)
    else()
        message(FATAL_ERROR "Raylib not found. Please ensure libs/raylib exists or install Raylib system-wide.")
    endif()
endif()

# Platform-specific system libraries
set(SYSTEM_LIBS "")

if(PLATFORM_WINDOWS)
    list(APPEND SYSTEM_LIBS winmm)
    
elseif(PLATFORM_LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    pkg_check_modules(XRANDR REQUIRED xrandr)
    pkg_check_modules(XI REQUIRED xi)
    pkg_check_modules(XCURSOR REQUIRED xcursor)
    pkg_check_modules(XINERAMA REQUIRED xinerama)
    
    list(APPEND SYSTEM_LIBS 
        ${X11_LIBRARIES}
        ${XRANDR_LIBRARIES}
        ${XI_LIBRARIES}
        ${XCURSOR_LIBRARIES}
        ${XINERAMA_LIBRARIES}
        dl
        pthread
        m
    )
    
elseif(PLATFORM_MACOS)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    find_library(COREAUDIO_LIBRARY CoreAudio REQUIRED)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox REQUIRED)
    
    list(APPEND SYSTEM_LIBS
        ${COCOA_LIBRARY}
        ${OPENGL_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${COREAUDIO_LIBRARY}
        ${AUDIOTOOLBOX_LIBRARY}
    )
endif()

# =============================================================================
# Source Files
# =============================================================================
file(GLOB_RECURSE PROJECT_SOURCES 
    CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.cxx"
    "src/*.cc"
)

file(GLOB_RECURSE PROJECT_HEADERS 
    CONFIGURE_DEPENDS
    "include/*.hpp"
    "include/*.h"
    "include/*.hxx"
)

# Resource files for Windows
set(PROJECT_RESOURCES "")
if(PLATFORM_WINDOWS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/app.rc")
    list(APPEND PROJECT_RESOURCES "resources/app.rc")
endif()

# Validate source files
if(NOT PROJECT_SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory")
endif()

# =============================================================================
# Executable Target
# =============================================================================
add_executable(${PROJECT_NAME} 
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_RESOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    EXPORT_NAME "${PROJECT_NAME}"
)

# Include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        ${RAYLIB_TARGET}
        ${SYSTEM_LIBS}
)

# =============================================================================
# Compiler Flags and Optimizations
# =============================================================================
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Build type definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG _DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# MSVC-specific flags
if(COMPILER_MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /utf-8
        $<$<CONFIG:Release>:/O2 /Oi /Ot /GL>
        $<$<CONFIG:Debug>:/Zi /Od>
    )
    
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/LTCG>
    )
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

# GCC/Clang-specific flags
if(COMPILER_GCC OR COMPILER_CLANG)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Release>:-O3 -march=native>
        $<$<CONFIG:Debug>:-g -O0>
    )
    
    # AddressSanitizer (optional, Debug only)
    if(ENABLE_ASAN)
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address>
        )
        message(STATUS "AddressSanitizer enabled for Debug builds")
    endif()
endif()

# =============================================================================
# Platform-Specific Configuration
# =============================================================================

# Windows-specific settings
if(PLATFORM_WINDOWS)
    # Hide console in Release builds
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<IF:$<CONFIG:Release>,TRUE,FALSE>
    )
    
    # Static linking for MinGW (portable executable)
    if(COMPILER_GCC)
        target_link_libraries(${PROJECT_NAME} PRIVATE 
            -static 
            -static-libgcc 
            -static-libstdc++
        )
    endif()
endif()

# macOS-specific settings
if(PLATFORM_MACOS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )
    
    # Copy Info.plist if it exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in"
        )
    endif()
endif()

# Linux-specific settings
if(PLATFORM_LINUX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "$ORIGIN:$ORIGIN/lib"
        BUILD_RPATH_USE_ORIGIN TRUE
    )
endif()

# =============================================================================
# Output Directories
# =============================================================================
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/$<CONFIG>"
)

file(MAKE_DIRECTORY "${OUTPUT_DIR}")

# =============================================================================
# Asset Management
# =============================================================================
set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_DEST_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

if(EXISTS "${ASSETS_SOURCE_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSETS_SOURCE_DIR}"
        "${ASSETS_DEST_DIR}"
        COMMENT "Copying assets to output directory"
    )
    message(STATUS "Assets will be copied from: ${ASSETS_SOURCE_DIR}")
else()
    message(WARNING "Assets directory not found: ${ASSETS_SOURCE_DIR}")
endif()

# =============================================================================
# IDE Integration
# =============================================================================
if(PROJECT_HEADERS)
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" 
        PREFIX "Header Files" 
        FILES ${PROJECT_HEADERS})
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" 
    PREFIX "Source Files" 
    FILES ${PROJECT_SOURCES})

if(PROJECT_RESOURCES)
    source_group("Resource Files" FILES ${PROJECT_RESOURCES})
endif()

# Set startup project for Visual Studio
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
        PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(EXISTS "${ASSETS_SOURCE_DIR}")
    install(DIRECTORY "${ASSETS_SOURCE_DIR}/"
        DESTINATION bin/assets
    )
endif()

# =============================================================================
# Custom Targets
# =============================================================================
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
    COMMENT "Cleaning all build artifacts"
)

add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Running ${PROJECT_NAME}"
)

# =============================================================================
# Build Information
# =============================================================================
message(STATUS "")
message(STATUS "=== Build Configuration Summary ===")
message(STATUS "Project Name:     ${PROJECT_NAME}")
message(STATUS "Project Version:  ${PROJECT_VERSION}")
message(STATUS "Build Type:       ${CMAKE_BUILD_TYPE}")
message(STATUS "Target Platform:  ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler:         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS "Output Directory: ${OUTPUT_DIR}")

list(LENGTH PROJECT_SOURCES SOURCE_COUNT)
list(LENGTH PROJECT_HEADERS HEADER_COUNT)
message(STATUS "Source Files:     ${SOURCE_COUNT}")
message(STATUS "Header Files:     ${HEADER_COUNT}")

if(PROJECT_RESOURCES)
    list(LENGTH PROJECT_RESOURCES RESOURCE_COUNT)
    message(STATUS "Resource Files:   ${RESOURCE_COUNT}")
endif()

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer: ENABLED (Debug only)")
else()
    message(STATUS "AddressSanitizer: DISABLED")
endif()

message(STATUS "===================================")