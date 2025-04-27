#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <algorithm>

// ===================
// 共用常數設定
// ===================
const int DEFAULT_BUFFER_CAPACITY = 10;
const int DEFAULT_NOC_SIZE      = 8;

// ===================
// 封包 (Packet) 結構 ---（第一週任務）
// ===================
struct Packet {
    int id;
    int source_x, source_y;
    int dest_x, dest_y;
};

// ===================
// Router 類別 ---（第一週任務）
// 每個 router 包含座標、最大 buffer 容量和封包緩衝區
// ===================
class Router {
public:
    int x, y;
    int bufferCapacity;
    std::queue<Packet> buffer;

    Router(int x_coord, int y_coord, int capacity = DEFAULT_BUFFER_CAPACITY)
        : x(x_coord), y(y_coord), bufferCapacity(capacity)
    {}

    // 計算擁塞率 = (當前封包數) / (buffer 最大容量)
    float getCongestion() const {
        return static_cast<float>(buffer.size()) / bufferCapacity;
    }

    // 新增封包到 buffer，成功則返回 true；若 buffer 滿則返回 false
    bool addPacket(const Packet& packet) {
        if (buffer.size() < bufferCapacity) {
            buffer.push(packet);
            return true;
        }
        return false;
    }

    // 移除 buffer 中最前端的封包
    void popPacket() {
        if (!buffer.empty()) {
            buffer.pop();
        }
    }

    // 檢查 buffer 是否有封包
    bool hasPacket() const {
        return !buffer.empty();
    }
};

// ===================
// NoC 類別 --- 組合所有 router 並實現模擬功能
// ===================
class NoC {
private:
    int size;            // 網格大小，預設 8
    std::vector<std::vector<Router>> grid;
    int packetCounter;   // 用來產生每個封包的唯一 id
    std::mt19937 rng;    // 隨機數產生器

public:
    // 使用者設定的 hotspot 區域（非集中式，可包含多個散落位置）
    std::vector<std::pair<int, int>> hotspotArea;
    bool useHotspotTraffic;

    // -------------------
    // 儲存每個 cycle 的 Load Balance Factor (LBF) 歷史，用於後續視覺化
    // -------------------
    std::vector<float> lbfHistory;

    // -------------------
    // 第一週：初始化 NoC (設置 8×8 網格及基礎設定)
    // -------------------
    NoC(int size = DEFAULT_NOC_SIZE)
        : size(size), packetCounter(0), useHotspotTraffic(false) {
        rng.seed(std::random_device()());
        for (int i = 0; i < size; i++) {
            std::vector<Router> row;
            for (int j = 0; j < size; j++) {
                row.emplace_back(i, j);
            }
            grid.push_back(row);
        }
    }

    // 設定 hotspot 區域 (第二週任務)
    void setHotspotArea(const std::vector<std::pair<int,int>>& area) {
        hotspotArea = area;
        useHotspotTraffic = true;
    }

    // 判斷某個座標是否位於 hotspot 區域
    bool isHotspot(int x, int y) const {
        return std::find(hotspotArea.begin(), hotspotArea.end(), std::make_pair(x, y))
               != hotspotArea.end();
    }

    // -------------------
    // Helper 函數：從所有非 hotspot 節點中隨機選擇一個目的地
    // -------------------
    std::pair<int,int> getRandomNonHotspotDestination() {
        std::vector<std::pair<int,int>> candidates;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (!isHotspot(i, j)) {
                    candidates.push_back({i, j});
                }
            }
        }
        if (candidates.empty()) {
            int rx = uniformInt(0, size - 1);
            int ry = uniformInt(0, size - 1);
            return {rx, ry};
        }
        int index = uniformInt(0, static_cast<int>(candidates.size()) - 1);
        return candidates[index];
    }

    // -------------------
    // 第二週：流量產生模組
    // 封包產生時，目的地一定選擇非 hotspot 節點
    // -------------------
    void generateTraffic() {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float prob = dist(rng);
        // 模擬每個 cycle 有 50% 機率生成封包
        if (prob < 0.5f) {
            int src_x = uniformInt(0, size - 1);
            int src_y = uniformInt(0, size - 1);

            // 目的地一定選擇非 hotspot 節點
            auto dest = getRandomNonHotspotDestination();
            int dest_x = dest.first;
            int dest_y = dest.second;

            // 避免 source 與 destination 相同
            if (src_x == dest_x && src_y == dest_y) {
                auto alt = getRandomNonHotspotDestination();
                dest_x = alt.first;
                dest_y = alt.second;
            }

            Packet p { packetCounter++, src_x, src_y, dest_x, dest_y };
            bool added = grid[src_x][src_y].addPacket(p);
            if (added)
                std::cout << "生成封包 ID " << p.id << " 於 (" << src_x << "," << src_y 
                          << ")，目的地 (" << dest_x << "," << dest_y << ")\n";
            else
                std::cout << "封包因 (" << src_x << "," << src_y << ") 的 buffer 滿而丟棄\n";
        }
    }

    // -------------------
    // 初始化 hotspot 區域負載：使每個 hotspot 至少有 7 個封包 (第二週任務補充)
    // -------------------
    void initializeHotspots() {
        for (const auto &coord : hotspotArea) {
            int x = coord.first;
            int y = coord.second;
            while (grid[x][y].buffer.size() < 7) {
                auto dest = getRandomNonHotspotDestination();
                if (dest.first == x && dest.second == y) {
                    dest = getRandomNonHotspotDestination();
                }
                Packet dummy { packetCounter++, x, y, dest.first, dest.second };
                bool added = grid[x][y].addPacket(dummy);
                if (added) {
                    std::cout << "Hotspot (" << x << "," << y << ") 預填 dummy 封包 ID " 
                              << dummy.id << "\n";
                } else {
                    break;
                }
            }
        }
    }

    // -------------------
    // 初始化非 hotspot 區域的原有負載 (使用隨機方式產生 0~4 封包)
    // -------------------
    void initializeNonHotspotLoads() {
        // 對每一個 router，若非 hotspot，就隨機產生 0~4 個 dummy 封包
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (!isHotspot(i, j)) {
                    int numPackets = uniformInt(0, 4);
                    for (int k = 0; k < numPackets; k++) {
                        auto dest = getRandomNonHotspotDestination();
                        if (dest.first == i && dest.second == j) {
                            dest = getRandomNonHotspotDestination();
                        }
                        Packet dummy { packetCounter++, i, j, dest.first, dest.second };
                        bool added = grid[i][j].addPacket(dummy);
                        if (added) {
                            std::cout << "非Hotspot (" << i << "," << j << ") 預填 dummy 封包 ID " 
                                      << dummy.id << "\n";
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }

    // ====================
    // 計算並回傳 Load Balance Factor (LBF)
    // LBF = max(c) / avg(c)
    // ====================
    float computeLBF() {
        float sum = 0.0f;
        float maxC = 0.0f;
        int N = size * size;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                float c = grid[i][j].getCongestion();
                sum += c;
                if (c > maxC) maxC = c;
            }
        }
        float avgC = (N > 0 ? sum / N : 0.0f);
        return maxC / (avgC > 1e-6f ? avgC : 1e-6f);
    }

    // -------------------
    // 模擬每個 cycle 的處理流程：流量產生 + 封包移動 + LBF 計算並儲存
    // -------------------
    void runSimulation(int numCycles) {
        for (int cycle = 0; cycle < numCycles; cycle++) {
            std::cout << "========== 第 " << cycle << " 週期 ==========\n";
            generateTraffic();
            // 封包移動（與 simulationStep 等效）
            struct Move { int cur_x, cur_y; Packet packet; int next_x, next_y; };
            std::vector<Move> moves;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    Router &router = grid[i][j];
                    if (!router.hasPacket()) continue;
                    Packet current = router.buffer.front();
                    if (current.dest_x == i && current.dest_y == j) {
                        router.popPacket();
                        std::cout << "封包 ID " << current.id << " 到達目的地 (" 
                                  << i << "," << j << ") 被移除\n";
                    } else {
                        int nx, ny;
                        if (getNextHop(i, j, current.dest_x, current.dest_y, nx, ny)) {
                            moves.push_back({i, j, current, nx, ny});
                        }
                    }
                }
            }
            for (auto &m : moves) {
                grid[m.cur_x][m.cur_y].popPacket();
                if (grid[m.next_x][m.next_y].addPacket(m.packet)) {
                    std::cout << "封包 ID " << m.packet.id 
                              << " 從 (" << m.cur_x << "," << m.cur_y 
                              << ") 移動到 (" << m.next_x << "," << m.next_y << ")\n";
                } else {
                    std::cout << "封包 ID " << m.packet.id 
                              << " 嘗試移動到 (" << m.next_x << "," << m.next_y 
                              << ") 但 buffer 滿，保留於 (" << m.cur_x << "," << m.cur_y << ")\n";
                    grid[m.cur_x][m.cur_y].addPacket(m.packet);
                }
            }
            // 計算並記錄 LBF
            float lbf = computeLBF();
            lbfHistory.push_back(lbf);
            std::cout << "LBF = " << lbf << "\n\n";
        }
    }

    // -------------------
    // 第三週：自適應路由演算法 (Adaptive Routing Decision)
    // 僅根據鄰近節點的 congestion 指標決定下一跳，符合 GitHub 中的說明
    // -------------------
    bool getNextHop(int cur_x, int cur_y, int dest_x, int dest_y, int &next_x, int &next_y) {
        std::vector<std::pair<int,int>> candidates;
        if (cur_x < dest_x && cur_x + 1 < size)
            candidates.push_back({cur_x + 1, cur_y});
        else if (cur_x > dest_x && cur_x - 1 >= 0)
            candidates.push_back({cur_x - 1, cur_y});
        if (cur_y < dest_y && cur_y + 1 < size)
            candidates.push_back({cur_x, cur_y + 1});
        else if (cur_y > dest_y && cur_y - 1 >= 0)
            candidates.push_back({cur_x, cur_y - 1});
        if (candidates.empty()) return false;
        float minCong = 1e9;
        std::tie(next_x, next_y) = candidates[0];
        for (auto &c : candidates) {
            float cong = grid[c.first][c.second].getCongestion();
            if (cong < minCong) {
                minCong = cong;
                next_x = c.first;
                next_y = c.second;
            }
        }
        return true;
    }

    // -------------------
    // 輔助函數：返回範圍 [low, high] 內的隨機整數
    // -------------------
    int uniformInt(int low, int high) {
        std::uniform_int_distribution<int> dist(low, high);
        return dist(rng);
    }
};


// ===================
// 主函數 --- 程式進入點
// ===================
int main() {
    // 第一週：初始化 8×8 NoC
    NoC noc(DEFAULT_NOC_SIZE);

    // 第二週：設定分散式的 hotspot 區域 (範例：6 個 hotspot 節點)
    std::vector<std::pair<int,int>> hotspot = {
        {1,2}, {2,5}, {3,4},
        {5,1}, {6,7}, {7,3}
    };
    noc.setHotspotArea(hotspot);

    // 初始化 hotspot 負載 (每個 hotspot 至少填充 7 個封包)
    noc.initializeHotspots();

    // 初始化非 hotspot 區域基線負載 (隨機產生 0~4 個封包)
    noc.initializeNonHotspotLoads();

    // 第三週：自適應路由演算法 (Adaptive Routing Decision) 已實作於 getNextHop()

    // 執行模擬：示範執行 10 個 cycle（可依需求調整）
    noc.runSimulation(10);

    return 0;
}
