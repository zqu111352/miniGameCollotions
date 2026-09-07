cmake_minimum_required(VERSION 3.10.0)
project(miniGameCollotions VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# 查找 SFML 包
# REQUIRED 表示如果找不到则报错停止
# COMPONENTS 指定需要的模块
#set(SFML_STATIC_LIBRARIES ON)
# When you try to locate the SFML libraries, you must specify which modules you want to use (System, Window, Graphics, Network, Audio, Main).
find_package(SFML 3 COMPONENTS System Window Graphics Audio)

# 获取 TGUI
set(USE_FETCH_CONTENT OFF)
set(TGUI_BACKEND SFML_GRAPHICS)
if(USE_FETCH_CONTENT)
    include(FetchContent)
    FetchContent_Declare(
        tgui
        GIT_REPOSITORY https://github.com/texus/TGUI.git
        GIT_TAG        1.x  # 请替换为最新的稳定版本号，如 1.0.0 或 master
    )
    FetchContent_MakeAvailable(tgui)
else()
    list(APPEND CMAKE_PREFIX_PATH 
        "F:/C++/TGUI-1.13/"
    )
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
    find_package(TGUI REQUIRED)
endif()
# 自动查找当前源码目录及其子目录下所有的 .cpp 和 .c 文件
file(GLOB_RECURSE SRC_FILES 
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

message(STATUS "TGUI_BACKEND: ${TGUI_BACKEND}")
add_executable(miniGameCollotions ${SRC_FILES})

target_include_directories(miniGameCollotions PRIVATE   
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SFML_INCLUDE_DIRS}
)

# TGUI::TGUI 是主库，TGUI::SFML_GRAPHICS 是 SFML 后端
target_link_libraries(miniGameCollotions PRIVATE 
    SFML::Graphics SFML::System SFML::Window SFML::Audio
    TGUI::TGUI)


# --- 自动复制 DLL 的逻辑 ---

# 更通用的方法：直接指定 DLL 路径（需根据你的实际构建路径调整）
# 如果是 FetchContent，路径通常在 _deps/tgui-build/bin/<Config>/tgui.dll
# 如果是安装版，路径在 ${TGUI_DIR}/../bin/tgui.dll

# 这里提供一个基于 SFML 的通用复制示例，TGUI 类似
function(copy_dlls TARGET_NAME)
    # 获取传入的所有 DLL 路径 (ARGN 包含所有未命名参数)
    set(DLL_LIST ${ARGN})
    
    foreach(DLL_PATH ${DLL_LIST})
        # 获取文件名，以便构造目标路径
        get_filename_component(DLL_NAME ${DLL_PATH} NAME)
        
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DLL_PATH}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/${DLL_NAME}"
            COMMENT "Copying ${DLL_NAME} to output directory"
            VERBATIM
        )
    endforeach()
endfunction()

# 示例：手动指定 SFML 和 TGUI 的 DLL 路径（请根据实际路径修改）
set(SFML_BIN_DIR "D:/Libraries/SFML-3.1.0/bin")
set(TGUI_BIN_DIR "F:/C++/TGUI-1.13/bin")

copy_dlls(miniGameCollotions 
    "${SFML_BIN_DIR}/sfml-system-d-3.dll"
    "${SFML_BIN_DIR}/sfml-window-d-3.dll"
    "${SFML_BIN_DIR}/sfml-graphics-d-3.dll"
    "${SFML_BIN_DIR}/sfml-audio-d-3.dll"
    "${TGUI_BIN_DIR}/tgui.dll"
)

# 将源码目录下的 font 文件夹复制到构建目录下的相同位置
add_custom_command(TARGET miniGameCollotions POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/font
    $<TARGET_FILE_DIR:miniGameCollotions>/font
)
# 将源码目录下的 res 文件夹复制到构建目录下的相同位置
add_custom_command(TARGET miniGameCollotions POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:miniGameCollotions>/res
)