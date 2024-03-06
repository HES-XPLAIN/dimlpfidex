#include "headers/tests.hpp"

void runDimlpTestsSuite() {
  testDensCls();
  testDimlpBT();
  testDimlpCls();
  testDimlpPred();
  testDimlpRul();
  testDimlpTrn();
}

void runFidextestsSuite() {
  testFidex();
  testFidexGlo();
  testFidexGloRules();
  testFidexGloStats();
}

int main() {
  runDimlpTestsSuite();
  runFidextestsSuite();
  runCommonTestSuite();
  return 0;
}
