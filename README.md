# [OOS-Pass](https://github.com/WYK15/OOS-Pass)

LLVM out of source示例，使用新的Pass模版编写。

项目的编写参考了以下资料：

- [Writing an LLVM Pass](https://llvm.org/docs/WritingAnLLVMNewPMPass.html)
- [llvm-tutor](https://github.com/banach-space/llvm-tutor)
- [Building LLVM with CMake](https://llvm.org/docs/CMake.html)



## 梗概

编写out of source的pass具有更高的移植性，且开发调试方便，每当LLVM升级时，直接移植out of source效率更高。

LLVM自 14 开始，开始使用新的Pass Manager，逐渐抛弃Legacy PassManager，新的Pass分为以下两类：

- Analasis Pass，分析Pass，通常不会改变输入的Module。继承自`AnalysisInfoMixin`
- Transformation Pass，变换Pass，通常会改变输入的Module。继承自`PassInfoMixin`

这个项目中只编写了用于新Pass Manager的Transformation Pass为例。即继承自`PassInfoMixin`的Pass。



## 项目架构

```shell
<project dir>/
    |
    CMakeLists.txt
    <lib>/
        |
        CMakeLists.txt
        Pass.cpp
        ...
    <include>/
    		Pass.h
```



## 编译out of source Pass

### 开发环境准备

- LLVM 15
-  CMake 3.13.4 or higher

In order to run the passes, you will need:

- **clang-15** (to generate input LLVM files)
- [**opt**](http://llvm.org/docs/CommandGuide/opt.html) (to run the passes)

通常这些环境通过编译LLVM即可都安装好，以Mac举例，编译LLVM并安装如下：

```shell
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
git checkout release/15.x
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" -DLLVM_TARGETS_TO_BUILD=host -DCMAKE_INSTALL_PREFIX=/path/to/install/dir  -DLLVM_ENABLE_PROJECTS=clang <llvm-project/root/dir>/llvm/
ninja && ninja install
```

### 编译项目步骤

```shell
cd <project dir>
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=<installation/dir/of/llvm/15> ../
make
```

cmake 部分其他参数介绍：

- -DCMAKE_BUILD_TYPE=Debug，编译为Debug版本，利于调试，发布可以编译为Release版本
- -G "Ninja"。指定编译为Ninja项目，也可以指定为"Xcode"，编译为Xcode项目。

### 运行Pass

使用`opt`运行Pass，首先使用`Clang`编译一份.ll文件/.bc文件

```shell
# Generate an LLVM test file
$LLVM_DIR/bin/clang -O1 -S -emit-llvm main.c -o main.ll
```

准备好.ll文件后，使用opt加载pass

```shell
# Run the pass
$LLVM_DIR/bin/opt -load-pass-plugin  lib/Debug/libLEO_PASSES.dylib  -passes=leo-test-pass -disable-output ./main.ll

# Expected output
PFunctionPass on : add
PFunctionPass on : sub
PFunctionPass on : main
```



## 调试Pass

### MAC上使用LLDB调试

使用lldb打开程序

```shell
lldb -- $LLVM_DIR/bin/opt -load-pass-plugin  lib/Debug/libLEO_PASSES.dylib  -passes=leo-test-pass -disable-output ./main.ll
(lldb) b PFunctionPass::run
(lldb) r
```

接着，lldb就会在`PFunctionPass::run`函数的入口断点

### 其他操作环境调试

使用GDB调试

```shell
gdb -- $LLVM_DIR/bin/opt -load-pass-plugin  lib/Debug/libLEO_PASSES.dylib  -passes=leo-test-pass -disable-output ./main.ll
(lldb) b PFunctionPass::run
(lldb) r
```



## 静态库 vs 动态库

项目默认编译为动态库，即`CMakeLists.txt`的配置中，`add_library`设置为 `MODULE`或者`SHARED`。

在MAC上默认会编译失败，需要加入

```shell
# Allow undefined symbols in shared objects on Darwin (this is the default
# behaviour on Linux)
target_link_libraries(LEO_PASSES
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
```



也可以指定使用`STATIC`编译为静态库，静态Plugin会被链接入可执行文件中，如`opt`，这样使用`opt`运行pass时不用指定`-load-pass-plugin`

详情参考[Dynamic vs Static Plugins](https://github.com/banach-space/llvm-tutor#dynamic-vs-static-plugins)



## 合并到源码中

开发完out of source pass后，若想集成到LLVM中，可以使用以下方式，参考自[官网介绍](https://llvm.org/docs/CMake.html#id19)

1. 添加以下代码到<project dir>/CMakeLists.txt中

   ```makefile
   list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
   include(AddLLVM)
   ```

2. 修改 `<project dir>/<lib>/CMakeLists.txt`为：

   ```makefile
   add_llvm_library(LLVMPassname MODULE
     Pass.cpp
   )
   ```

   