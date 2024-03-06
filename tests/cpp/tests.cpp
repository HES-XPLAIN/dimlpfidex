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
  std::cout << "Running tests... (can take a while to finish)" << endl;
  runDimlpTestsSuite();
  runFidextestsSuite();
  runCommonTestSuite();
  std::cout << "Tests done." << endl;
  return 0;
}
