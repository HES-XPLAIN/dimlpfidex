#include "headers/tests.hpp"

uint runDimlpTestsSuite() {
  uint res = 0;

  res += testDimlpTrn();
  res += testDimlpBT();
  res += testDensCls();
  res += testDimlpCls();
  res += testDimlpPred();
  res += testDimlpRul();

  return res;
}

// test avec dimlpTrn weights file
uint runFidextestsSuite() {
  uint res = 0;

  res += testFidex();
  res += testFidexGloRules();
  res += testFidexGlo();
  res += testFidexGloStats();

  return res;
}

// TODO test avec dimlpBT weights file

int main() {
  uint res = 0;
  std::cout << "Running tests... (can take a while to finish)" << std::endl;

  res += runDimlpTestsSuite();
  res += runFidextestsSuite();
  res += runCommonTestSuite();

  std::cout << res << " tests done." << std::endl;
  return 0;
}
