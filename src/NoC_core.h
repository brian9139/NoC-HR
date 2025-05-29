#pragma once

#include <vector>
#include <queue>
#include <random>
#include <algorithm>

// ========== 常數 ==========
static constexpr int DEFAULT_BUFFER_CAPACITY = 10;
static constexpr int DEFAULT_NOC_SIZE      = 8;

// ========== Packet 結構 ==========
struct Packet {
    int id;
    int source_x, source_y;
    int dest_x, dest_y;
};

// ========== Router ==========
class Router {
public:
    int x, y;
    int bufferCapacity;
    std::queue<Packet> buffer;

    Router(int x_coord, int y_coord, int capacity = DEFAULT_BUFFER_CAPACITY);

    float getCongestion() const;
    bool addPacket(const Packet& packet);
    void popPacket();
    bool hasPacket() const;
};

// ========== NoC ==========
class NoC {
public:
    // 讓 Python binding 可以直接讀取
    std::vector<std::vector<Router>> grid;
    std::vector<std::pair<int,int>> hotspotArea;
    bool useHotspotTraffic;
    std::vector<float> lbfHistory;

    NoC(int size = DEFAULT_NOC_SIZE);

    void setHotspotArea(const std::vector<std::pair<int,int>>& area);
    bool isHotspot(int x, int y) const;
    std::pair<int,int> getRandomNonHotspotDestination();
    void generateTraffic();
    void initializeHotspots();
    void initializeNonHotspotLoads();
    float computeLBF();
    void runSimulation(int numCycles);
    bool getNextHop(int cur_x,int cur_y,int dest_x,int dest_y,int& next_x,int& next_y);
    int uniformInt(int low, int high);

private:
    int size;
    int packetCounter;
    std::mt19937 rng;
};
