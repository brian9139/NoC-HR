#include <gtest/gtest.h>
#include "NoC_core.cpp"

// 原有測試：預設初始化參數
TEST(NoCInitialization, DefaultParameters) {
    NoC noc;
    EXPECT_TRUE(noc.lbfHistory.empty());
    float lbf = noc.computeLBF();
    EXPECT_FLOAT_EQ(lbf, 0.0f);
}

// 新增測試：預設網格大小為 DEFAULT_NOC_SIZE x DEFAULT_NOC_SIZE
TEST(NoCInitialization, DefaultGridSize) {
    NoC noc;
    EXPECT_EQ(noc.grid.size(), static_cast<size_t>(DEFAULT_NOC_SIZE));
    EXPECT_EQ(noc.grid[0].size(), static_cast<size_t>(DEFAULT_NOC_SIZE));
}

// 新增測試：自訂大小 NoC(4) 產生 4×4 網格
TEST(NoCInitialization, CustomGridSize) {
    NoC noc4(4);
    EXPECT_EQ(noc4.grid.size(), 4u);
    EXPECT_EQ(noc4.grid[0].size(), 4u);
}

// 新增測試：最小尺寸 NoC(1) 行為
TEST(NoCInitialization, OneByOneGrid) {
    NoC noc1(1);
    EXPECT_EQ(noc1.grid.size(), 1u);
    EXPECT_EQ(noc1.grid[0].size(), 1u);
    // 空緩衝，LBF 應為 0
    EXPECT_FLOAT_EQ(noc1.computeLBF(), 0.0f);
}

// 新增測試：零尺寸 NoC(0) 行為
TEST(NoCInitialization, ZeroSizeGrid) {
    NoC noc0(0);
    EXPECT_TRUE(noc0.grid.empty());
    // 無 router，LBF 定義為 0
    EXPECT_FLOAT_EQ(noc0.computeLBF(), 0.0f);
}

// 新增測試：runSimulation 會填滿 lbfHistory
TEST(NoCSimulation, LBFHistoryLength) {
    NoC noc;
    noc.runSimulation(7);
    EXPECT_EQ(noc.lbfHistory.size(), 7u);
    // 執行完畢後所有值都應該存在
    for (float v : noc.lbfHistory) {
        EXPECT_GE(v, 0.0f);
    }
}
