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
public:
    // 將 grid 設為 public，以便單元測試直接訪問
    std::vector<std::vector<Router>> grid;

    // 使用者設定的 hotspot 區域（非集中式，可包含多個散落位置）
    std::vector<std::pair<int, int>> hotspotArea;
    bool useHotspotTraffic;

    // 儲存每個 cycle 的 Load Balance Factor (LBF) 歷史，用於後續視覺化
    std::vector<float> lbfHistory;

private:
    int size;            // 網格大小，預設 8
    int packetCounter;   // 用來產生每個封包的唯一 id
    std::mt19937 rng;    // 隨機數產生器

public:
    // -------------------
    // 第一週：初始化 NoC (設置 8×8 網格及基礎設定)
    // -------------------
    NoC(int s = DEFAULT_NOC_SIZE)
        : size(s), packetCounter(0), useHotspotTraffic(false)
    {
        rng.seed(std::random_device()());
        // 建立 grid
        if (size > 0) {
            grid.reserve(size);
            for (int i = 0; i < size; i++) {
                std::vector<Router> row;
                row.reserve(size);
                for (int j = 0; j < size; j++) {
                    row.emplace_back(i, j);
                }
                grid.push_back(std::move(row));
            }
        }
    }

    // 設定 hotspot 區域 (第二週任務)
    void setHotspotArea(const std::vector<std::pair<int,int>>& area) {
        hotspotArea = area;
        useHotspotTraffic = true;
    }

    // 判斷某個座標是否位於 hotspot 區域
    bool isHotspot(int x, int y) const {
        return std::find(hotspotArea.begin(), hotspotArea.end(),
                         std::make_pair(x, y))
               != hotspotArea.end();
    }

    // -------------------
    // Helper 函數：從所有非 hotspot 節點中隨機選擇一個目的地
    // -------------------
    std::pair<int,int> getRandomNonHotspotDestination() {
        std::vector<std::pair<int,int>> candidates;
        candidates.reserve(size * size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (!isHotspot(i, j)) candidates.emplace_back(i, j);
            }
        }
        if (candidates.empty()) {
            // 退化情況
            return { uniformInt(0, size-1), uniformInt(0, size-1) };
        }
        return candidates[ uniformInt(0, static_cast<int>(candidates.size())-1) ];
    }

    // -------------------
    // 第二週：流量產生模組
    // 封包產生時，目的地一定選擇非 hotspot 節點
    // -------------------
void generateTraffic() {
    if (size <= 0) return;
    std::uniform_real_distribution<float> coin(0,1);
    for (int sx = 0; sx < size; sx++) {
        for (int sy = 0; sy < size; sy++) {
            // 每個 router 20% 機率 inject
            if (coin(rng) < 0.2f) {
                auto [dx,dy] = getRandomNonHotspotDestination();
                Packet p{ packetCounter++, sx, sy, dx, dy };
                grid[sx][sy].addPacket(p);
            }
        }
    }
}

    // -------------------
    // 初始化 hotspot 區域負載：使每個 hotspot 至少有 7 個封包
    // -------------------
    void initializeHotspots() {
        if (size <= 0) return;
        for (auto [x,y] : hotspotArea) {
            if (x<0||y<0||x>=size||y>=size) continue;
            while (grid[x][y].buffer.size() < 7) {
                auto [dx,dy] = getRandomNonHotspotDestination();
                if (dx==x && dy==y)
                    std::tie(dx,dy) = getRandomNonHotspotDestination();
                Packet d{ packetCounter++, x, y, dx, dy };
                if (!grid[x][y].addPacket(d)) break;
            }
        }
    }

    // -------------------
    // 初始化非 hotspot 區域的原有負載 (隨機產生 0~4 個封包)
    // -------------------
    void initializeNonHotspotLoads() {
        if (size <= 0) return;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (isHotspot(i,j)) continue;
                int cnt = uniformInt(0,4);
                for (int k = 0; k < cnt; k++) {
                    auto [dx,dy] = getRandomNonHotspotDestination();
                    if (dx==i && dy==j)
                        std::tie(dx,dy) = getRandomNonHotspotDestination();
                    Packet d{ packetCounter++, i, j, dx, dy };
                    if (!grid[i][j].addPacket(d)) break;
                }
            }
        }
    }

    // ====================
    // 計算並回傳 Load Balance Factor (LBF)
    // LBF = max(congestion) / avg(congestion)
    // ====================
    float computeLBF() {
        float sum = 0.0f, maxC = 0.0f;
        int N = size * size;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                float c = grid[i][j].getCongestion();
                sum += c;
                if (c > maxC) maxC = c;
            }
        }
        float avg = (N > 0 ? sum / N : 0.0f);
        return maxC / (avg > 1e-6f ? avg : 1e-6f);
    }

    // -------------------
    // 執行模擬：每 cycle 流量產生 + 封包移動 + LBF 計算並儲存
    // -------------------
    void runSimulation(int cycles) {
        for (int c = 0; c < cycles; c++) {
            generateTraffic();

            struct Move { int cx, cy; Packet p; int nx, ny; };
            std::vector<Move> moves;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    auto &r = grid[i][j];
                    if (!r.hasPacket()) continue;
                    Packet cur = r.buffer.front();
                    if (cur.dest_x == i && cur.dest_y == j) {
                        r.popPacket();
                    } else {
                        int nx, ny;
                        if (getNextHop(i,j,cur.dest_x,cur.dest_y,nx,ny))
                            moves.push_back({i,j,cur,nx,ny});
                    }
                }
            }
            for (auto &m : moves) {
                grid[m.cx][m.cy].popPacket();
                grid[m.nx][m.ny].addPacket(m.p);
            }

            lbfHistory.push_back(computeLBF());
        }
    }

    // -------------------
    // 第三週：Adaptive Routing Decision
    // 僅根據鄰近節點的 congestion 指標選擇下一跳
    // -------------------
    bool getNextHop(int cx, int cy, int dx, int dy, int &next_x, int &next_y) {
        std::vector<std::pair<int,int>> cand;
        if (cx < dx) cand.emplace_back(cx+1, cy);
        else if (cx > dx) cand.emplace_back(cx-1, cy);
        if (cy < dy) cand.emplace_back(cx, cy+1);
        else if (cy > dy) cand.emplace_back(cx, cy-1);
        if (cand.empty()) return false;
        float best = 1e9;
        std::tie(next_x, next_y) = cand[0];
        for (auto &p : cand) {
            float c = grid[p.first][p.second].getCongestion();
            if (c < best) {
                best = c;
                next_x = p.first;
                next_y = p.second;
            }
        }
        return true;
    }

    // -------------------
    // 輔助函數：返回範圍 [low, high] 內的隨機整數
    // 若 high<low，直接回傳 low
    // -------------------
    int uniformInt(int low, int high) {
        if (high < low) return low;
        std::uniform_int_distribution<int> dist(low, high);
        return dist(rng);
    }
};

// ===================
// 主程式
// ===================
int main() {
    NoC noc;
    noc.setHotspotArea({{1,2},{2,5},{3,4},{5,1},{6,7},{7,3}});
    noc.initializeHotspots();
    noc.initializeNonHotspotLoads();
    noc.runSimulation(10);
    return 0;
}
