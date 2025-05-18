#include <gtest/gtest.h>
#include "NoC_core.cpp"

// 原有測試：hotspot 排除
TEST(TrafficGeneration, HotspotExclusion) {
    NoC noc;
    noc.setHotspotArea({{0,0}});
    for (int i = 0; i < 1000; ++i) {
        noc.generateTraffic();
    }
    EXPECT_EQ(noc.grid[0][0].buffer.size(), 0u);
}

// 新增測試：非 hotspot 節點至少注入一次封包
TEST(TrafficGeneration, NonHotspotInjection) {
    NoC noc;
    // 預設 hotspotArea 為空 => 全網格可注入
    bool saw = false;
    for (int i = 0; i < 1000 && !saw; ++i) {
        noc.generateTraffic();
        if (!noc.grid[0][0].buffer.empty())
            saw = true;
    }
    EXPECT_TRUE(saw);
}

// 新增測試：多重 hotspot 排除
TEST(TrafficGeneration, MultipleHotspots) {
    NoC noc;
    std::vector<std::pair<int,int>> hs = {{0,0},{1,1},{2,2}};
    noc.setHotspotArea(hs);
    for (int i = 0; i < 1000; ++i) {
        noc.generateTraffic();
    }
    for (auto [x,y] : hs) {
        EXPECT_EQ(noc.grid[x][y].buffer.size(), 0u);
    }
}

// 新增測試：若不設定 hotspot （useHotspotTraffic=false），也應在所有位置隨機注入
TEST(TrafficGeneration, NoHotspot_AllNodesPossible) {
    NoC noc;
    // 不呼叫 setHotspotArea, hotspotArea 保持空
    std::set<std::pair<int,int>> seen;
    for (int i = 0; i < 2000 && seen.size() < DEFAULT_NOC_SIZE*DEFAULT_NOC_SIZE; ++i) {
        noc.generateTraffic();
        for (int x = 0; x < DEFAULT_NOC_SIZE; ++x)
            for (int y = 0; y < DEFAULT_NOC_SIZE; ++y)
                if (!noc.grid[x][y].buffer.empty())
                    seen.insert({x,y});
    }
    // 理論上所有節點最終都有機會被注入
    EXPECT_EQ(seen.size(), DEFAULT_NOC_SIZE * DEFAULT_NOC_SIZE);
}
