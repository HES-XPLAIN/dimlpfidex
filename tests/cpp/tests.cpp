#include "headers/tests.hpp"

uint runDimlpTestsSuite() {
  uint res = 0;

  res += testDimlpBT();
  res += testDensCls();
  res += testDimlpCls();
  res += testDimlpPred();
  res += testDimlpRul();
  res += testDimlpTrn();

  return res;
}

uint runFidextestsSuite() {
  uint res = 0;

  res += testFidex();
  res += testFidexGlo();
  res += testFidexGloRules();
  res += testFidexGloStats();

  return res;
}

int main() {
  uint res = 0;
  std::cout << "Running tests... (can take a while to finish)" << endl;

  res += runDimlpTestsSuite();
  res += runFidextestsSuite();
  res += runCommonTestSuite();

  std::cout << res << " tests done." << endl;
  return 0;
}
