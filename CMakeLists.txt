cmake_minimum_required(VERSION 3.14)
project(NoC_Simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Pybind11（Python 綁定）
# ----------------------------
find_package(pybind11 REQUIRED)

# ----------------------------
# Core 函式庫
# ----------------------------
add_library(noc_core STATIC
    ${CMAKE_SOURCE_DIR}/src/NoC_core.cpp
    ${CMAKE_SOURCE_DIR}/src/NoC_core.h
)
target_include_directories(noc_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)
set_target_properties(noc_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ----------------------------
# Python 模組
# ----------------------------
pybind11_add_module(noc_sim
    ${CMAKE_SOURCE_DIR}/src/pybind11_module.cpp
)
target_link_libraries(noc_sim PRIVATE noc_core)

# ----------------------------
# GoogleTest，建測試執行檔前先 FetchContent
# ----------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ----------------------------
# 需要 pthread
# ----------------------------
find_package(Threads REQUIRED)

# ----------------------------
# 啟用 CTest
# ----------------------------
enable_testing()

# ----------------------------
# 測試一：NoC 初始化
# ----------------------------
add_executable(test_noc_init
    ${CMAKE_SOURCE_DIR}/test/test_noc_initialization.cpp
)
target_include_directories(test_noc_init PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(test_noc_init
    PRIVATE noc_core gtest gtest_main Threads::Threads
)
add_test(NAME NoC_Init_Test COMMAND test_noc_init)

# ----------------------------
# 測試二：流量生成
# ----------------------------
add_executable(test_traffic_gen
    ${CMAKE_SOURCE_DIR}/test/test_traffic_generation.cpp
)
target_include_directories(test_traffic_gen PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(test_traffic_gen
    PRIVATE noc_core gtest gtest_main Threads::Threads
)
add_test(NAME Traffic_Gen_Test COMMAND test_traffic_gen)

# ----------------------------
# 測試三：路由與擁塞指標
# ----------------------------
add_executable(test_routing_metrics
    ${CMAKE_SOURCE_DIR}/test/test_routing_and_congestion.cpp
)
target_include_directories(test_routing_metrics PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(test_routing_metrics
    PRIVATE noc_core gtest gtest_main Threads::Threads
)
add_test(NAME Routing_Metrics_Test COMMAND test_routing_metrics)
