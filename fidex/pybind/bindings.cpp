#include "../cpp/src/fidexFct.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(fidex, m) {
  m.doc() = "pybind11 fidex plugin"; // optional module docstring
  m.def("fidex", &fidex, "A function to get explanation rule for one or many particular samples.");
}
