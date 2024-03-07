#include "../cpp/src/densClsFct.h"
#include "../cpp/src/dimlpBTFct.h"
#include "../cpp/src/dimlpClsFct.h"
#include "../cpp/src/dimlpPredFct.h"
#include "../cpp/src/dimlpRulFct.h"
#include "../cpp/src/dimlpTrnFct.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(dimlp, m) {
  m.doc() = "pybind11 dimlp plugin"; // optional module docstring
  m.def("dimlpPred", &dimlpPred, pybind11::arg("command") = "", "A function to get predictions on test set, training needs to be done before.");
  m.def("dimlpCls", &dimlpCls, pybind11::arg("command") = "", "A function to get predictions and accuracy on test set, need to give test classess and training needs to be done before.");
  m.def("dimlpRul", &dimlpRul, pybind11::arg("command") = "", "A function to get rules with stats for train, test and validation sets, gives also accuracy and error.");
  m.def("dimlpTrn", &dimlpTrn, pybind11::arg("command") = "", "A function to train dimlp model on training set, get accuracies and errors on train, test and validation sets, get the model weights and the predictions on train and test sets. Extract rules with stats for each set");
  m.def("dimlpBT", &dimlpBT, pybind11::arg("command") = "", "A function to train N dimlp models with bagging on training set, get accuracies and errors on train, test and validation sets, get the model weights and the predictions on train and test sets. Extract rules with stats for each set");
  m.def("densCls", &densCls, pybind11::arg("command") = "", "A function to get rules and precision and global accuracy on train and test on a bag of models, training needs to be done before.");
}
