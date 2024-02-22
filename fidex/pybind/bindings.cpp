#include "../cpp/src/fidexFct.h"
#include "../cpp/src/fidexGloFct.h"
#include "../cpp/src/fidexGloRulesFct.h"
#include "../cpp/src/fidexGloStatsFct.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(fidex, m) {
  m.doc() = "pybind11 fidex plugin"; // optional module docstring
  m.def("fidex", &fidex, "A function to get explanation rule for one or many particular samples.");
  m.def("fidexGlo", &fidexGlo, "A function to get explanation rule for one or many particular samples using pretrained rules.");
  m.def("fidexGloRules", &fidexGloRules, "A function to get rules from a train dataSet.");
  m.def("fidexGloStats", &fidexGloStats, "A function to get statistics from pretrained rules.");
}
