# add_llvm_library(LEO_PASSES MODULE
#     PFunctionPass.cpp
# )

add_library(LEO_PASSES SHARED 
    PFunctionPass.cpp
)

# CMAKE_CURRENT_SOURCE_DIR是一个CMake内置的变量，它表示当前处理的CMakeLists.txt所在的目录路径。
set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")
include_directories(${HEADER_DIR})


# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
target_link_libraries(LEO_PASSES
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")