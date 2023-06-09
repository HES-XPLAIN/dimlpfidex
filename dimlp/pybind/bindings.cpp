#include "../cpp/src/DensClsFct.h"
#include "../cpp/src/DimlpBTFct.h"
#include "../cpp/src/DimlpClsFct.h"
#include "../cpp/src/DimlpPredFct.h"
#include "../cpp/src/DimlpRulFct.h"
#include "../cpp/src/DimlpTrnFct.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(dimlp, m) {
  m.doc() = "pybind11 dimlp plugin"; // optional module docstring
  m.def("dimlpPred", &dimlpPred, "A function to get predictions on test set, training needs to be done before.");
  m.def("dimlpCls", &dimlpCls, "A function to get predictions and accuracy on test set, need to give test classess and training needs to be done before.");
  m.def("dimlpRul", &dimlpRul, "A function to get rules with stats for train, test and validation sets, gives also accuracy and error.");
  m.def("dimlpTrn", &dimlpTrn, "A function to train dimlp model on training set, get accuracies and errors on train, test and validation sets, get the model weights and the predictions on train and test sets. Extract rules with stats for each set");
  m.def("dimlpBT", &dimlpBT, "A function to train N dimlp models with bagging on training set, get accuracies and errors on train, test and validation sets, get the model weights and the predictions on train and test sets. Extract rules with stats for each set");
  m.def("densCls", &densCls, "A function to get rules and precision and global accuracy on train and test on a bag of models, training needs to be done before.");
}
