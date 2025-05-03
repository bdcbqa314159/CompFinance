#include "aad_ingestion" //IWYU pragma: keep

#include <cstddef>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void init_node(py::module &m) {
  py::class_<Node>(m, "Node")
      .def(py::init<const size_t>(), py::arg("n") = 0)
      .def("adjoint", py::overload_cast<>(&Node::adjoint))
      .def("adjoint", py::overload_cast<const size_t>(&Node::adjoint))
      .def("propagateOne", &Node::propagateOne)
      .def("propagateAll", &Node::propagateAll);
}
