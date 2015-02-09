# ================================================================
# CMake definition file
#
# The defines below determine how cmake generates our nimble 
# library, executables and where / how to find dependencies. By
# default calling cmake with this CMakeLists.txt will generate
# project files for the native system (eg. NIMBLE_TARGET_WIN32, osx), linked
# against prebuilt external libs (eg. bullet, curl, ...etc.).
# 
# ================================================================

cmake_minimum_required(VERSION 2.8)
project(nimbleimageloaderfreeimage)

# ================================================================

macro(header_directories path return_list)
    file(GLOB_RECURSE new_list ${path})
    set(dir_list "")
    foreach(file_path ${new_list})
        get_filename_component(dir_path ${file_path} PATH)
        set(dir_list ${dir_list} ${dir_path})
    endforeach()
    list(REMOVE_DUPLICATES dir_list)
    set(${return_list} ${dir_list})
endmacro()

macro(configure_msvc_runtime)
  if(MSVC)
    # Default to statically-linked runtime.
    if("${MSVC_RUNTIME}" STREQUAL "")
      set(MSVC_RUNTIME "static")
    endif()
    # Set compiler options.
    set(variables
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    if(${MSVC_RUNTIME} STREQUAL "static")
      message(STATUS
        "MSVC -> forcing use of statically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
        endif()
      endforeach()
    else()
      message(STATUS
        "MSVC -> forcing use of dynamically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MT")
          string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
        endif()
      endforeach()
    endif()
  endif()
endmacro()

configure_msvc_runtime()

set(variables
  CMAKE_C_FLAGS_DEBUG
  CMAKE_C_FLAGS_MINSIZEREL
  CMAKE_C_FLAGS_RELEASE
  CMAKE_C_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_MINSIZEREL
  CMAKE_CXX_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_RELWITHDEBINFO
)

# ================================================================

if(NIMBLE_SHOW_CMAKE_VARS)
    # shows all available defined variables
    get_cmake_property(_variableNames VARIABLES)
    foreach(_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()

    message(STATUS "Initial build flags:")
    foreach(variable ${variables})
      message(STATUS "  '${variable}': ${${variable}}")
    endforeach()
endif()

# ================================================================
# Determine where to find our FindXXX.cmake files

# FindXXX.cmake files define XXX_INCLUDE_DIRS, XXX_LIBS, XXX_DEFINITIONS
if(NOT DEFINED CMAKE_MODULE_PATH)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
endif()

# ================================================================

#message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
#message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
#message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
#message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

# ================================================================

if(NIMBLE_TARGET_WIN32)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DNIMBLE_DEBUG -g -W4")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNIMBLE_RELEASE -W4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DNIMBLE_DEBUG -g -W4")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNIMBLE_RELEASE -W4")
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DNIMBLE_DEBUG -g -Wno-deprecated-declarations")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNIMBLE_RELEASE -Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DNIMBLE_DEBUG -g -Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNIMBLE_RELEASE -Wno-deprecated-declarations")
endif()
if(NIMBLE_ENABLE_COVERAGEFLAGS)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DNIMBLE_DEBUG -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNIMBLE_RELEASE -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DNIMBLE_DEBUG -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNIMBLE_RELEASE -fprofile-arcs -ftest-coverage")
endif()
if(NIMBLE_TARGET_IOS)
    set(SDKVER "7.1")
    set(DEVROOT "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer")
    set(SDKROOT "${DEVROOT}/SDKs/iPhoneOS${SDKVER}.sdk")
    set(CMAKE_OSX_SYSROOT "${SDKROOT}")
    set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_32_BIT) i386 x86_64 arm64")
    set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    add_definitions(-DNIMBLE_TARGET_IOS)
elseif(NIMBLE_TARGET_OSX)
    set(SDKVER "10.9")
    set(DEVROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer")
    set(SDKROOT "${DEVROOT}/SDKs/MacOSX${SDKVER}.sdk")
    set(CMAKE_OSX_SYSROOT "${SDKROOT}")
    if(NOT NIMBLE_GENPROJ_TESTS)
        set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_32_BIT) x86_64")
    endif()
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    add_definitions(-DNIMBLE_TARGET_OSX)
endif()
if(NIMBLE_TARGET_WIN32)
    add_definitions(-DNOMINMAX) # removes window defines for min / max
    add_definitions(/wd4127)    # removes warning (conditional expression is constant)
    add_definitions(/wd4100)    # removes warning (unreferenced parameter)
    add_definitions(/wd4522)    # removes warning (multiple assignment operators found)
    add_definitions(/wd4180)    # removes warning (qualifier applied to function type has no meaning)
    add_definitions(-DNIMBLE_TARGET_WIN32)
endif()
add_definitions(-DFREEIMAGE_LIB)
add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)
add_definitions(-DUSE_DL_PREFIX=1)

if(NIMBLE_DISABLE_ASSERTS)
    add_definitions(-DNDEBUG=1)
endif()

# ================================================================
# Find and include external packages
# ================================================================
message(STATUS "Looking for external packages...")

find_package(nimble REQUIRED)
include_directories(${nimble_INCLUDE_DIRS})
message(STATUS ${nimble_INCLUDE_DIRS})

find_package(freeimage REQUIRED)
include_directories(${freeimage_INCLUDE_DIRS})
message(STATUS ${freeimage_INCLUDE_DIRS})

# ================================================================
# Generate nimble lib
# ================================================================

# gather our source and header files
file(GLOB_RECURSE Src ./source/*.cpp)
file(GLOB_RECURSE Inc ./include/*.h)

# ================================================================

# create our static nimble library
include_directories(./include)
include_directories(./source)
include_directories(./sdk)
set(Files ${Src} ${Inc})
list(SORT Files)
add_library(nimbleimageloaderfreeimage ${Files})

# ================================================================
