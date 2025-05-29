import os
import sys
import pytest

# 把 build/ 加到模組搜尋路徑
HERE = os.path.dirname(__file__)
BUILD_DIR = os.path.abspath(os.path.join(HERE, '..', 'build'))
if BUILD_DIR not in sys.path:
    sys.path.insert(0, BUILD_DIR)

import noc_sim

@pytest.fixture
def small_noc():
    return noc_sim.NoCSimulator(4)

def test_import_and_basic_methods(small_noc):
    noc = small_noc
    for name in [
        "set_hotspot_area", "generate_traffic", "initialize_hotspots",
        "initialize_non_hotspots", "run_simulation", "compute_lbf",
        "get_lbf_history", "get_congestion_grid"
    ]:
        assert hasattr(noc, name)

def test_congestion_grid_initial(small_noc):
    grid = small_noc.get_congestion_grid()
    # 初始時所有 congestion 都應該是 0
    for row in grid:
        for c in row:
            assert c == pytest.approx(0.0)

def test_initialize_hotspots_and_nonhotspots(small_noc):
    noc = small_noc
    hotspots = [(0, 1), (2, 2)]
    noc.set_hotspot_area(hotspots)

    # 初始化 hotspot
    noc.initialize_hotspots()
    grid1 = noc.get_congestion_grid()
    # hotspot 至少要有 7/10 = 0.7 以上 (允許小幅度誤差)
    for x, y in hotspots:
        assert grid1[x][y] >= 0.7 - 1e-3

    # 初始化非 hotspot 負載
    noc.initialize_non_hotspots()
    grid2 = noc.get_congestion_grid()
    # 非 hotspot 節點載具比例應介於 0.0 到 0.4 之間
    for i in range(4):
        for j in range(4):
            if (i, j) not in hotspots:
                assert 0.0 <= grid2[i][j] <= 0.4 + 1e-6

def test_run_simulation_and_lbf_history(small_noc):
    noc = small_noc
    cycles = 5
    noc.run_simulation(cycles)
    history = noc.get_lbf_history()
    assert isinstance(history, list)
    assert len(history) == cycles
    # LBF 都不應該是負數
    for v in history:
        assert v >= 0.0

def test_congestion_grid_shape_and_bounds():
    for size in [0, 1, 3]:
        noc = noc_sim.NoCSimulator(size)
        grid = noc.get_congestion_grid()
        assert isinstance(grid, list)
        assert len(grid) == size
        assert all(len(row) == size for row in grid)
        # 值範圍在 [0,1]
        for row in grid:
            for c in row:
                assert 0.0 <= c <= 1.0

@pytest.mark.parametrize("size", [0, 1, 2])
def test_various_sizes_lbf_history(size):
    noc = noc_sim.NoCSimulator(size)
    noc.run_simulation(3)
    h = noc.get_lbf_history()
    assert len(h) == 3
    assert all(isinstance(v, float) and v >= 0.0 for v in h)
