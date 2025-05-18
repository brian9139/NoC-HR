#include <gtest/gtest.h>
#include "NoC_core.cpp"

// 原有測試：Uniform Congestion => LBF ≈ 1
TEST(LoadBalanceFactor, UniformCongestion) {
    NoC noc;
    for (int i = 0; i < DEFAULT_NOC_SIZE; ++i) {
        for (int j = 0; j < DEFAULT_NOC_SIZE; ++j) {
            for (int k = 0; k < 5; ++k) {
                noc.grid[i][j].addPacket({k, i, j, (i+1)%DEFAULT_NOC_SIZE, (j+1)%DEFAULT_NOC_SIZE});
            }
        }
    }
    float lbf = noc.computeLBF();
    EXPECT_NEAR(lbf, 1.0f, 1e-3);
}

// 新增測試：只有一個 router 滿，其它空 => LBF = N
TEST(LoadBalanceFactor, SingleFullRouter) {
    NoC noc;
    int N = DEFAULT_NOC_SIZE * DEFAULT_NOC_SIZE;
    // 把 (0,0) 塞滿
    for (int k = 0; k < DEFAULT_BUFFER_CAPACITY; ++k) {
        noc.grid[0][0].addPacket({k,0,0,1,1});
    }
    float lbf = noc.computeLBF();
    EXPECT_NEAR(lbf, static_cast<float>(N), 1e-3);
}

// 原有測試：避開更擁塞的鄰居
TEST(RoutingDecision, LeastCongestedPath) {
    NoC noc;
    noc.grid[1][0].addPacket({0,0,0,2,0});
    noc.grid[1][0].addPacket({1,0,0,2,0});
    int nx, ny;
    ASSERT_TRUE(noc.getNextHop(0,0,0,2,nx,ny));
    EXPECT_EQ(nx, 0);
    EXPECT_EQ(ny, 1);
}

// 新增測試：二方向都可選時，正確比較
TEST(RoutingDecision, BothDirectionsComparison) {
    NoC noc;
    noc.grid[1][0].addPacket({0,0,0,1,1});
    noc.grid[1][0].addPacket({1,0,0,1,1});
    int nx, ny;
    ASSERT_TRUE(noc.getNextHop(0,0,1,1,nx,ny));
    EXPECT_EQ(nx, 0);
    EXPECT_EQ(ny, 1);
}

// 新增測試：只有 X 方向可選
TEST(RoutingDecision, OnlyXDirection) {
    NoC noc;
    int nx, ny;
    ASSERT_TRUE(noc.getNextHop(0,0,2,0,nx,ny));
    EXPECT_EQ(nx, 1);
    EXPECT_EQ(ny, 0);
}

// 新增測試：只有 Y 方向可選
TEST(RoutingDecision, OnlyYDirection) {
    NoC noc;
    int nx, ny;
    ASSERT_TRUE(noc.getNextHop(0,0,0,3,nx,ny));
    EXPECT_EQ(nx, 0);
    EXPECT_EQ(ny, 1);
}

// 原有測試：無候選方向時回傳 false
TEST(RoutingDecision, NoCandidate) {
    NoC noc;
    int nx, ny;
    EXPECT_FALSE(noc.getNextHop(0,0,0,0,nx,ny));
}

// 原有測試：Router.getCongestion()
TEST(CongestionRatio, RouterGetCongestion) {
    Router r(0,0,10);
    for (int i = 0; i < 5; ++i)
        r.addPacket({i,0,0,1,1});
    EXPECT_FLOAT_EQ(r.getCongestion(), 0.5f);
}

// 新增測試：多跳模擬檢查 pipeline 行為
TEST(RoutingPipeline, MultiHopMovement) {
    NoC noc;
    // 人為注入一個封包到 (0,0)，目的 (2,0)
    Packet p{0,0,0,2,0};
    noc.grid[0][0].addPacket(p);
    // Cycle 1：應該從 (0,0) 移到 (1,0)
    noc.runSimulation(1);
    EXPECT_TRUE(noc.grid[1][0].hasPacket());
    // Cycle 2：再移到 (2,0)，並被移除
    noc.runSimulation(1);
    EXPECT_FALSE(noc.grid[2][0].hasPacket());
}

// 新增測試：runSimulation 動態 LBF 走勢合理
TEST(LoadBalanceHistory, MonotonicOrFluctuating) {
    NoC noc;
    // 小規模模擬
    noc.runSimulation(5);
    // 檢查長度
    EXPECT_EQ(noc.lbfHistory.size(), 5u);
    // 確保所有值非負
    for (auto v : noc.lbfHistory)
        EXPECT_GE(v, 0.0f);
}
