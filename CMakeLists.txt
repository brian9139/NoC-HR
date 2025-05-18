cmake_minimum_required(VERSION 3.14)
project(NoC_Simulator_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# 自動 clone 並編譯 GoogleTest
# ----------------------------
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# 避免覆蓋父專案的 CRT 設定（Windows）
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ----------------------------
# 包含自己專案的原始碼目錄
# ----------------------------
include_directories(${CMAKE_SOURCE_DIR})

# ----------------------------
# 建構 NoC core 靜態函式庫
# ----------------------------
add_library(noc_core STATIC
    ${CMAKE_SOURCE_DIR}/NoC_core.cpp   # 對應你的原始碼檔案
)

# ----------------------------
# 引入執行緒函式庫
# ----------------------------
find_package(Threads REQUIRED)

# ----------------------------
# 啟用測試發現
# ----------------------------
enable_testing()

# ----------------------------
# 測試一：NoC 初始化
# ----------------------------
add_executable(test_noc_init
    ${CMAKE_SOURCE_DIR}/test/test_noc_initialization.cpp
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
target_link_libraries(test_routing_metrics
    PRIVATE noc_core gtest gtest_main Threads::Threads
)
add_test(NAME Routing_Metrics_Test COMMAND test_routing_metrics)
