cmake_minimum_required(VERSION 3.13.4)
project(leo-llvm-test)

#===============================================================================
# 1. VERIFY LLVM INSTALLATION DIR
# This is just a bit of a sanity checking.
#===============================================================================
set(LT_LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")

# 1.1 Check the "include" directory
set(LT_LLVM_INCLUDE_DIR "${LT_LLVM_INSTALL_DIR}/include/llvm")
if(NOT EXISTS "${LT_LLVM_INCLUDE_DIR}")
message(FATAL_ERROR
  " LT_LLVM_INSTALL_DIR (${LT_LLVM_INCLUDE_DIR}) is invalid.")
endif()


# 1.2 Check that the LLVMConfig.cmake file exists (the location depends on the
# OS)
set(LT_VALID_INSTALLATION FALSE)

# Ubuntu + Darwin
if(EXISTS "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

# Fedora
if(EXISTS "${LT_LLVM_INSTALL_DIR}/lib64/cmake/llvm/LLVMConfig.cmake")
  set(LT_VALID_INSTALLATION TRUE)
endif()

if(NOT ${LT_VALID_INSTALLATION})
  message(FATAL_ERROR
    "LLVM installation directory, (${LT_LLVM_INSTALL_DIR}), is invalid. Couldn't
    find LLVMConfig.cmake.")
endif()


#===============================================================================
# 2. LOAD LLVM CONFIGURATION
#    For more: http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
#===============================================================================
# Add the location of LLVMConfig.cmake to CMake search paths (so that
# find_package can locate it)
# Note: On Fedora, when using the pre-compiled binaries installed with `dnf`,
# LLVMConfig.cmake is located in "/usr/lib64/cmake/llvm". But this path is
# among other paths that will be checked by default when using
# `find_package(llvm)`. So there's no need to add it here.
find_package(LLVM REQUIRED CONFIG)

#list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")
#include(AddLLVM)


# Set the LLVM header and library paths
set(LLVM_INCLUDE_DIRS "${LT_LLVM_INSTALL_DIR}/include")
set(LLVM_LIBRARY_DIRS "${LT_LLVM_INSTALL_DIR}/lib")
set(LLVM_DEFINITIONS "-fno-rtti")

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Another sanity check
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LT_LLVM_INSTALL_DIR}")

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}
  Targets     ${LLVM_TARGETS_TO_BUILD}"
)


#===============================================================================
# 3. LLVM-TUTOR BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

# Build type
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall\
  -fdiagnostics-color=always")

# LLVM is normally built without RTTI. Be consistent with that.
# -fno-rtti是GCC和Clang编译器提供的一个选项，用于控制是否启用运行时类型信息（RTTI）支持。默认情况下，编译器会生成RTTI支持代码，以便在运行时检查对象的类型信息。
# 启用RTTI的主要好处是在进行动态类型识别时可以提供更好的支持。但是，这也会导致生成更大的可执行文件，并且会增加程序的运行时间和内存开销。
# 使用-fno-rtti选项可以告诉编译器不要生成RTTI支持代码，以便减小可执行文件的大小并降低程序的运行时间和内存开销。但是，这也会使得在进行动态类型识别时可能会出现问题。
# 在使用LLVM进行编译时，-fno-rtti选项通常用于减小LLVM生成的可执行文件的大小，或者在需要嵌入LLVM库的应用程序中减小库的大小。但是，这可能会影响到某些LLVM库的正确性，因此需要仔细考虑是否启用该选项。
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()


# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-tutor is built without this flag (though otherwise it
# builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if (${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# Set the build directories
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_subdirectory(lib)