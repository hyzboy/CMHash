# CMHash Examples
#
# 刻意不使用 cm_example_project_base：那个宏会强制链接 CMCore/CMUtil，
# 而 CMHash 的边界就是「只依赖 CMCoreType」。示例要能证明这一点。

add_executable(WyHashTest WyHashTest.cpp)

target_link_libraries(WyHashTest PRIVATE CMCoreType)
target_include_directories(WyHashTest PRIVATE ${CMHASH_ROOT_INCLUDE_PATH})

set_target_properties(WyHashTest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    FOLDER "Examples/CMHash")

if(WIN32 AND DEFINED HGL_MATH_LIB)
    target_link_libraries(WyHashTest PRIVATE ${HGL_MATH_LIB})
    set_debugger_directory(WyHashTest ${CMAKE_CURRENT_SOURCE_DIR})
endif()
