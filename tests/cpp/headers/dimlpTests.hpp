#ifndef DIMLP_TESTS_H
#define DIMLP_TESTS_H

#include "utils.hpp"
#include "../../../dimlp/cpp/src/densClsFct.h"
#include "../../../dimlp/cpp/src/dimlpBTFct.h"
#include "../../../dimlp/cpp/src/dimlpClsFct.h"
#include "../../../dimlp/cpp/src/dimlpPredFct.h"
#include "../../../dimlp/cpp/src/dimlpRulFct.h"
#include "../../../dimlp/cpp/src/dimlpTrnFct.h"

const std::string TEMPLATES_PATH = "tests/templates/";
void testDensCls();
void testDimlpBT();
void testDimlpCls();
void testDimlpPred();
void testDimlpRul();
void testDimlpTrn();

#endif // DIMLP_TESTS_H