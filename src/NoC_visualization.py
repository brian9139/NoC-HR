import os
import sys

# -----------------------------------------------------------------------------
# 1. 把 build/ 加到 Python 模組搜尋路徑
# -----------------------------------------------------------------------------
HERE = os.path.dirname(__file__)
BUILD_DIR = os.path.abspath(os.path.join(HERE, '..', 'build'))
if BUILD_DIR not in sys.path:
    sys.path.insert(0, BUILD_DIR)

import matplotlib
# 如果在無 GUI 環境下：
# matplotlib.use('Agg')

import matplotlib.pyplot as plt
from matplotlib import gridspec
import noc_sim

# -----------------------------------------------------------------------------
# 2. 收集快照與 LBF（僅針對自適應路由）
# -----------------------------------------------------------------------------
def collect_data(noc, total_cycles, snapshot_interval, hotspot_area=None):
    # hotspot 與 non-hotspot 的預設初始化
    if hotspot_area:
        noc.set_hotspot_area(hotspot_area)
        noc.initialize_hotspots()
        noc.initialize_non_hotspots()
    else:
        noc.initialize_non_hotspots()

    snapshots = []
    lbf_history = []

    for cycle in range(1, total_cycles + 1):
        noc.run_simulation(1)
        lbf_history.append(noc.get_lbf_history()[-1])
        if cycle % snapshot_interval == 0:
            snapshots.append(noc.get_congestion_grid())
    return snapshots, lbf_history

# -----------------------------------------------------------------------------
# 3. 顯示一組熱力圖快照網格
# -----------------------------------------------------------------------------
def show_snapshots(snapshots, snapshot_interval):
    n = len(snapshots)                # 應該是 10
    cols = 5
    rows = (n + cols - 1) // cols     # = 2

    fig = plt.figure(figsize=(4*cols, 4*rows))
    # 使用 GridSpec 為 colorbar 留位置
    gs = gridspec.GridSpec(rows, cols+1, width_ratios=[1]*cols + [0.2])
    
    for idx, grid in enumerate(snapshots):
        r, c = divmod(idx, cols)
        ax = fig.add_subplot(gs[r, c])
        im = ax.imshow(grid, origin='lower',
                       cmap='viridis', vmin=0, vmax=1)
        ax.set_title(f"Cycle {snapshot_interval*(idx+1)}", fontsize=12)
        ax.set_xticks([]); ax.set_yticks([])

    # 在最右側並跨兩行放置 colorbar
    cax = fig.add_subplot(gs[:, cols])
    fig.colorbar(im, cax=cax, label="Congestion Ratio")

    plt.tight_layout()
    plt.subplots_adjust(right=0.90)  # 給 colorbar 留地方
    plt.show()
    
# -----------------------------------------------------------------------------
# 4. 繪製自適應路由下的 LBF 折線圖
# -----------------------------------------------------------------------------
def plot_lbf(lbf_history):
    cycles = list(range(1, len(lbf_history) + 1))
    plt.figure(figsize=(8,4))
    plt.plot(cycles, lbf_history, marker='o', label="自適應路由")
    plt.title("LBF 隨時間變化（自適應路由）")
    plt.xlabel("Cycle")
    plt.ylabel("LBF")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.show()

# -----------------------------------------------------------------------------
# 5. 綜合流程
# -----------------------------------------------------------------------------
def visualize(size=8,
              total_cycles=50,
              snapshot_interval=10,
              hotspot_area=None):
    # 僅使用自適應路由模擬
    noc = noc_sim.NoCSimulator(size=size)

    snapshots, lbf_history = collect_data(
        noc, total_cycles, snapshot_interval, hotspot_area
    )
    show_snapshots(snapshots, snapshot_interval)
    plot_lbf(lbf_history)

# -----------------------------------------------------------------------------
# 6. 腳本入口
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    # hotspot 範例
    hotspot_nodes = [(1,2), (2,5), (3,4), (5,1), (6,7), (7,3)]
    visualize(
        size=8,
        total_cycles=50,
        snapshot_interval=5,
        hotspot_area=hotspot_nodes
    )
