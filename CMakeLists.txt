# ----------------------------------------------------------------------------
#  Root CMake file for Rockchip Media Process Platform (MPP)
#
#   - 10:34 2015/7/27: Initial version <herman.chen@rock-chips.com>
#
#   - 15:40 2025/8/1: 修改整体框架
#
# ----------------------------------------------------------------------------
project(tta_mpp)
cmake_minimum_required(VERSION 3.10)

# CMAKE_BUILD_TYPE初始默认值为Debug
if(NOT CMAKE_BUILD_TYPE)
    # default to Release build for GCC builds
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) 、Debug 、Release 、RelWithDebInfo 、MinSizeRel."
        FORCE)
endif()
message(STATUS "cmake version ------ ${CMAKE_VERSION}")

# 搜索主机上的工具包，而不是目标机上的工具包
# 在交叉编译时，这些宏应该由工具链定义好
# 如果没有存在find_host_package与find_host_program，定义一个宏，使其调用find_package
if(NOT COMMAND find_host_package)
    macro(find_host_package)
        find_package(${ARGN})
    endmacro()
endif()
if(NOT COMMAND find_host_program)
    macro(find_host_program)
        find_program(${ARGN})
    endmacro()
endif()

# 提供check_include_files()命令，用于检查指定的头文件是否存在
include(CheckIncludeFiles)
# 提供check_function_exists()命令，用于检查指定的函数是否存在于系统库中
include(CheckFunctionExists)
# CheckSymbolExists：提供check_symbol_exists()命令，用于检查指定的符号是否在指定的头文件中定义
include(CheckSymbolExists)
# CheckCXXCompilerFlag：提供check_cxx_compiler_flag()命令，用于检查 C++ 编译器是否支持某个特定的编译标志
include(CheckCXXCompilerFlag)

##########################################################################################################################################
##                                                            编译库                                                                    ##
##########################################################################################################################################
# 设置MPP驱动动/静态库名称（MPP Media Process Platform 媒体处理平台）
if (NOT MPP_PROJECT_NAME)
    set(MPP_PROJECT_NAME rockchip_mpp)
endif()
set(MPP_STATIC_LIB ${MPP_PROJECT_NAME}_static)
set(MPP_SHARED_LIB ${MPP_PROJECT_NAME}_shared)

# 设置VPU驱动动/静态库名称 (VPU Video Process Unit 视频处理单元)
if (NOT VPU_PROJECT_NAME)
    set(VPU_PROJECT_NAME rockchip_vpu)
endif()
set(VPU_STATIC_LIB ${VPU_PROJECT_NAME}_STATIC)
set(VPU_SHARED_LIB ${VPU_PROJECT_NAME}_SHARED)

# 启用文件夹功能，允许将目标（targets）和源文件组织到逻辑文件夹中
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# 将 CMake 自动生成的预定义目标（如 ALL_BUILD、INSTALL、ZERO_CHECK 等）统一放入名为 "CMakeTargets" 的文件夹中
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# 设置一个编译选项，关系着是只生成库还是库与执行文件
option(BUILD_BIN "enable test binary building)" ON)

# 启用编译命令导出功能，CMake 会在构建目录中生成一个名为compile_commands.json的文件
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

##########################################################################################################################################
##                                                         系统架构检测                                                                  ##
##########################################################################################################################################
# 检测当前编译系统
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" BUILD_SYSPROC)
message(STATUS "Build system processor: ${BUILD_SYSPROC}")
# 设置X86交叉编译选项
set(X86_ALIASE x86 i386 i686 x86_64 amd64)
list(FIND X86_ALIASE "${BUILD_SYSPROC}" X86_MATCH)
# 没有buildroot的开发经验，而且官方貌似做了兼容
if ("${CMAKE_C_COMPILER}" MATCHES "-buildroot-[^/]+$")
    message(STATUS "Detected Buildroot toolchain")
    # Use buildroot toolchain's default architecture settings
elseif ("${BUILD_SYSPROC}" STREQUAL "" OR X86MATCH GREATER "-1")
    message(STATUS "Detected x86 system processor")
    set(X86 true)
    add_definitions(-DARCH_X86=1)
    if("${CMAKE_SIZEOF_VOID_P}" MATCHES 8)
        set(X64 true)
        add_definitions(-DARCH_X64=1)
        message(STATUS "Define X86_64 to 1")
    endif()
elseif(${BUILD_SYSPROC} STREQUAL "armv6l")
    message(STATUS "Detected ARMv6 system processor")
    set(ARM true)
    set(ARMEABI_V6 true)
elseif(${BUILD_SYSPROC} STREQUAL "armv7-a")
    message(STATUS "Detected ARMv7 system processor")
    set(ARM true)
    set(ARMEABI_V7A true)
elseif(${BUILD_SYSPROC} STREQUAL "armv7-a_hardfp" OR ${BUILD_SYSPROC} STREQUAL "armv7l")
    message(STATUS "Detected ARMv7 system processor")
    set(ARM true)
    set(ARMEABI_V7A_HARDFP true)
elseif(${BUILD_SYSPROC} STREQUAL "aarch64" OR ${BUILD_SYSPROC} STREQUAL "armv8-a")
    message(STATUS "Detected ARMv8 system processor")
    set(ARM true)
    set(ARMEABI_V8 true)
else()
    message(STATUS "CMAKE_SYSTEM_PROCESSOR value `${CMAKE_SYSTEM_PROCESSOR}` is unknown")
    message(STATUS "Please add this value near ${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE}")
endif()

if(UNIX)
    message(STATUS "Build system : ${CMAKE_SYSTEM_NAME}")
    set(PLATFORM_LIBS pthread)
    if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(PLATFORM_LIBS ${PLATFORM_LIBS} rt)
    endif()
endif(UNIX)

##########################################################################################################################################
##                                                           编译检测                                                                    ##
##########################################################################################################################################
# 检查编译链类型
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CLANG true)
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(INTEL_CXX true)
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(GCC true)
endif()
message(STATUS "Build toolchain: ${CMAKE_CXX_COMPILER_ID}")

# 设置编译选项
# -Wall：启用编译器的基本警告（"all" 的缩写，但并非所有警告）
# -Wextra：启用额外的警告选项，比-Wall更严格
# -Wshadow：当一个变量遮蔽（隐藏）了另一个变量时发出警告（例如局部变量与全局变量同名）
# -ffast-math：启用快速数学优化，可能会牺牲一些浮点数精度和严格标准符合性来提高性能
if(INTEL_CXX AND UNIX)
    # treat icpc roughly like gcc
    set(GCC true)
    add_definitions(-Wall -Wextra -Wshadow)
elseif(CLANG)
    # treat clang roughly like gcc
    set(GCC true)
    add_definitions(-Wall -Wextra -Wshadow -ffast-math)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall -Wextra -Wshadow -ffast-math)
    # 检查当前 C++ 编译器是否支持-Wno-narrowing标志（该标志用于禁用 "数据窄化" 警告）。
    check_cxx_compiler_flag(-Wno-narrowing GCC_HAS_NO_NARROWING)
    # 检查编译器是否支持-mstackrealign标志（该标志用于强制栈对齐，在某些架构或安全场景下需要）。
    check_cxx_compiler_flag(-mstackrealign GCC_HAS_STACK_REALIGN)
    if (GCC_HAS_STACK_REALIGN)
        #  GCC 等编译器的选项，用于强制栈的重新对齐
        add_definitions(-mstackrealign)
    endif()
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
endif()

# 编译器优化选项
if(GCC)
    if(ARM)
        if(ARMEABI_V6)
            # -march=armv6：指定目标 ARM 架构版本为 ARMv6，编译器会针对该架构的特性进行优化
            # -mfloat-abi=hard：设置硬件浮点 ABI（应用程序二进制接口），使用硬件浮点单元进行浮点运算，性能更好
            # -mfpu=vfp：指定使用 VFP（Vector Floating Point，向量浮点）协处理器，启用 VFP 浮点指令集
            add_definitions(-march=armv6 -mfloat-abi=hard -mfpu=vfp)
        elseif(ARMEABI_V7A)
            # -mfpu=neon：启用 NEON 多媒体处理单元，这是 ARM 的 SIMD（单指令多数据）扩展，用于加速多媒体和信号处理等计算密集型任务
            add_definitions(-march=armv7-a -mfloat-abi=softfp -mfpu=neon)
        elseif(ARMEABI_V7A_HARDFP)
            add_definitions(-march=armv7-a -mfloat-abi=hard -mfpu=neon)
        elseif(ARMEABI_V8)
            # -march=armv8：指定目标 ARM 架构版本为 ARMv8-a，编译器会针对该架构的特性进行优化
            add_definitions(-march=armv8-a)
        endif()
    else()
        if(X86 AND NOT X64)
            add_definitions(-march=i686)
        endif()
    endif()

    if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Release")
        # 其作用是生成包含调试信息的可执行文件或目标文件。这些调试信息包括变量名、函数名、源代码行号等，能够帮助调试器（如 gdb）定位问题和跟踪程序执行流程。
        add_definitions(-g)
    endif()

    # -Wno-multichar是 GCC、Clang 等编译器支持的选项，作用是禁用 “多字符常量” 警告。
    add_definitions(-Wno-multichar)
    # -fPIC是 GCC、Clang 等编译器支持的选项，全称是 "Position-Independent Code"（位置无关代码）。它生成的机器码不依赖于特定的内存地址，可以被加载到内存的任意位置执行。
    # 创建共享库（.so/.dll）：大多数系统要求共享库必须使用-fPIC编译，否则可能无法链接
    # 动态加载代码：对于需要在运行时动态加载的代码（如插件），通常也需要位置无关特性
    add_definitions(-fPIC)
    # -fno-exceptions 禁用 C++ 异常处理机制。编译时不会生成与异常相关的代码（如 try/catch 块的处理逻辑）
    # -fno-rtti 禁用 C++ 的 RTTI（Run-Time Type Information，运行时类型信息）
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
    # save intermediate files
    # add_definitions(-save-temps)

    # 这是一个传递给链接器的选项，作用是告诉链接器：在遇到下一个--no-whole-archive选项之前，将所有指定的静态库（.a 文件）中的所有目标文件都链接到最终的二进制文件中
    set(BEGIN_WHOLE_ARCHIVE -Wl,--whole-archive)
    # 该选项用于关闭--whole-archive的功能，恢复链接器默认的行为（只链接被引用的目标文件）
    set(END_WHOLE_ARCHIVE -Wl,--no-whole-archive)

    option(ASAN_CHECK "enable Address Sanitizer (Asan)" OFF)
    if(ASAN_CHECK)
        add_definitions(-fsanitize=address -static-libasan -g)
        set(ASAN_LIB libasan.a dl rt m)
        set(ASAN_BIN dl rt m)
    endif(ASAN_CHECK)
endif(GCC)

##########################################################################################################################################
##                                                           git版本维护                                                                ##
##########################################################################################################################################
# 初始化github仓库历史版本条目
set(VERSION_CNT         0)
set(VERSION_MAX_CNT     9)
set(VERSION_INFO        "\"unknown mpp version for missing VCS info\"")
foreach (CNT RANGE ${VERSION_MAX_CNT})
    set(VERSION_HISTORY_${CNT} "NULL")
endforeach(CNT)

# 检测隐藏文件夹是否存在
if (EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # 寻找git包
    find_host_package(Git)
    if (GIT_FOUND)
        set(GIT_LOG_FORMAT "%h author: %<|(30)%an %cd %s")
    endif(GIT_FOUND)

endif(EXISTS "${PROJECT_SOURCE_DIR}/.git")





