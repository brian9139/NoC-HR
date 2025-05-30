# NoC-HR

Adaptive Hotspot Routing for NoC Simulation

## Basic Information

**GitHub:** <https://github.com/brian9139/NoC-HR.git>

## Problem to Solve

Network-on-Chip (NoC) is a key technology for modern many-core processors
and chiplet-based architectures. Common small-scale NoCs, such as those in
multi-core CPUs or AI accelerators, are mostly 4×4 or 8×8. As for
communication, the network communication between chiplets is the bottleneck
of system communication compared to intra-chiplet communication.

Furthermore, when NoC design is load-imbalanced, where certain regions of
the network become hotspots due to high traffic, these hotspots lead to
increased latency and reduced throughput.

This project focuses on implementing an adaptive routing algorithm in a
software-based NoC simulator to tackle hotspot congestion, thereby
decreasing the load imbalance factor (LBF).

## Prospective Users

- Computer architects studying NoC performance optimization.
- Chip designers looking for efficient routing strategies for chiplet-based
  architectures.
- Researchers developing novel adaptive routing algorithms.
- Students learning about NoC behavior and performance metrics.

## System Architecture

Three formulas for evaluating NoC load imbalance:

1. **Congestion calculation:**

   ```math
   C(R) = \frac{\text{buffer maximum capacity}}{\text{number of packets in the buffer}}
   ```

   This equation represents the buffer congestion ratio, which measures how
   full a buffer is relative to its maximum capacity.

2. **Adaptive routing decision:**

   ```math
   R_{\mathrm{next}}
   =
   \arg\min_{R_j \in \mathrm{XY\text{-}Neighbors}(R_i)}
   \Bigl[
   C(R_j)
   +
   \lambda \,\sqrt{(x_j - x_{\mathrm{dest}})^2 + (y_j - y_{\mathrm{dest}})^2}
   \Bigr]
   ```

   This method compares the congestion levels of the four neighboring nodes
   of the current node and selects the one with the lowest congestion as the
   next hop. This helps avoid entering hotspot areas, reducing the overall
   system burden.

3. **Load balance factor (LBF):**

   ```math
   LBF = \frac{\max C(R)}{\frac{1}{N} \sum C(R)}
   ```

   The LBF measures how evenly traffic is distributed across the network.
   It is defined as the ratio between the maximum congestion in the network
   and the average congestion across all routers. A higher LBF indicates
   higher load imbalance.

### Implementation Plan

First, in C++ an 8×8 2D Network-on-Chip (NoC) will be created, where each
node is defined as a router. The system will be initialized with relevant
information for each node, such as maximum capacity and current load. A
hotspot scenario will be generated, aiming for an LBF value of
approximately 1.5–2.

Next, the NoC adaptive routing function will be implemented in C++.
Additionally, at each cycle, packets will be randomly generated at a random
node and sent to another random destination node, simulating real-world
traffic conditions. During the dynamic simulation, the changes in LBF will
be observed, and the effectiveness of this adaptive routing method in
mitigating system congestion under hotspot conditions will be evaluated.

Finally, Pybind11 will be used to bind the C++ core to Python, with Python
handling the initialization and visualization aspects of the simulation.

## API Description

The system exposes an API for users to configure and run NoC simulations.

### Python Interface:

```python
from noc_sim import NoCSimulator

# Initialize NoC
noc = NoCSimulator(size=8, routing_algorithm="adaptive")

# Set traffic pattern
noc.generate_traffic(mode="hotspot", hotspot_area=[(3,3), (3,4), (4,3), (4,4)])

# Run simulation
noc.run_simulation(num_cycles=1000)

# Evaluate performance
noc.visualize_congestion()  # Displays congestion levels in the 8×8 NoC
noc.visualize_performance_metrics()  # Plots LBF and latency over time
```

## Engineering Infrastructure

- **Programming languages:** Python (simulation, visualization), C++ (adaptive
  routing core)
- **Build system:** CMake for C++, Python setuptools for Python wrapper
- **Version control:** Git
- **Testing framework:** Google Test (C++), pytest (Python)

## Schedule

```md
| Week  | Date   | Task                                   |
|-------|--------|---------------------------------------|
| 1     | 03/24  | Define data structure & setup       |
| 2     | 03/31  | Implement traffic generation model  |
| 3     | 04/07  | Implement adaptive routing function  |
| 4     | 04/14  | Unit testing: NoC initialization, traffic generation |
| 5     | 04/21  | Unit testing: routing decisions, congestion metrics  |
| 6     | 04/28  | Bind C++ NoC core to Python (Pybind11) |
| 7     | 05/05  | Implement visualization module     |
| 8     | 05/12  | Performance evaluation & metrics   |
| 9     | 05/19  | System integration & optimizations |
| 10    | 05/26  | Prepare presentation               |
```

