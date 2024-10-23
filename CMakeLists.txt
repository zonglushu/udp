# CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(UDP_EXAMPLE)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
  ${PROJECT_SOURCE_DIR}/include/
  ${PROJECT_SOURCE_DIR}/utils/
  ${PROJECT_SOURCE_DIR}/rapidjson/
  )

include_directories(${Boost_INCLUDE_DIRS})  # 包含Boost头文件路径

file(GLOB UTILS_SRCS "utils/**/*.cpp")

set(UDP_LINUX_LIB_SRCS
    tte_linux_lib.cpp
    easylogging++.cc
    ${UTILS_SRCS}
    )


set(TTE_LINUX_LIB_SRCS_LIB
    tte_linux_lib.cpp
    tte_linux_lib_c_wrapper.cpp
    ${UTILS_SRCS}
    easylogging++.cc
  )

find_package(Boost REQUIRED COMPONENTS system filesystem serialization thread chrono) # 指定需要的组件

add_subdirectory(xlnt)

# 打包为动态库,cmake会自动往前拼接lib前缀
add_library(myTTE SHARED ${TTE_LINUX_LIB_SRCS_LIB})  

target_link_libraries(myTTE  xlnt)

# 可执行文件
add_executable(udp_test udp_test.cpp hw_library_handle.cpp)

# 链接动态库
target_link_libraries(udp_test myTTE)


# add_executable(json_test json_test.cpp ${UDP_LINUX_LIB_SRCS})
# add_executable(UDPServer udp_server.cpp)
# add_executable(UDPClient udp_client.cpp)
# target_link_libraries(json_test xlnt)
# target_link_libraries(json_test ${Boost_LIBRARIES})
# # 链接 Boost 库
# target_link_libraries(UDPServer ${Boost_LIBRARIES})
# target_link_libraries(UDPClient ${Boost_LIBRARIES})

# # 查找源文件
# set(SOURCES
#     udp_server.cpp
#     udp_client.cpp
# )

# # 创建可执行文件
# add_executable(udp_server udp_server.cpp)
# add_executable(udp_client udp_client.cpp)

# # 可选：设置编译选项（如有需要）
# target_compile_options(udp_server PRIVATE -Wall -Wextra)
# target_compile_options(udp_client PRIVATE -Wall -Wextra)
