#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "NoC_core.h"

namespace py = pybind11;

PYBIND11_MODULE(noc_sim, m) {
    m.doc() = "Python bindings for C++ NoC simulator";

    py::class_<Packet>(m, "Packet")
        .def_readonly("id", &Packet::id)
        .def_readonly("source_x", &Packet::source_x)
        .def_readonly("source_y", &Packet::source_y)
        .def_readonly("dest_x", &Packet::dest_x)
        .def_readonly("dest_y", &Packet::dest_y);

    py::class_<Router>(m, "Router")
        .def_readonly("x", &Router::x)
        .def_readonly("y", &Router::y)
        .def("get_congestion", &Router::getCongestion)
        .def("has_packet", &Router::hasPacket)
        .def_property_readonly("buffer_size", [](const Router &r){ return r.buffer.size(); });

    py::class_<NoC>(m, "NoCSimulator")
        .def(py::init<int>(), py::arg("size") = DEFAULT_NOC_SIZE)
        .def("set_hotspot_area", &NoC::setHotspotArea)
        .def("generate_traffic", &NoC::generateTraffic)
        .def("initialize_hotspots", &NoC::initializeHotspots)
        .def("initialize_non_hotspots", &NoC::initializeNonHotspotLoads)
        .def("run_simulation", &NoC::runSimulation, py::arg("num_cycles"))
        .def("compute_lbf", &NoC::computeLBF)
        .def("get_lbf_history", [](const NoC &n){ return n.lbfHistory; })
        .def("get_congestion_grid", [](const NoC &n){
            std::vector<std::vector<float>> mat(n.grid.size());
            for (size_t i=0; i<n.grid.size(); ++i) {
                mat[i].resize(n.grid[i].size());
                for (size_t j=0; j<n.grid[i].size(); ++j)
                    mat[i][j] = n.grid[i][j].getCongestion();
            }
            return mat;
        });
}