#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_node(py::module &);

namespace py_add_code {

PYBIND11_MODULE(my_python_package, m) {

  m.doc() = "python binding for aad code";

  init_node(m);
}
} // namespace py_add_code