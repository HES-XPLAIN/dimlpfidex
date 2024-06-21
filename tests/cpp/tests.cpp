#include "headers/tests.hpp"

int runDimlpTestsSuite() {
  int res = 0;

  res += testDimlpTrn();
  res += testDimlpBT();
  res += testDensCls();
  res += testDimlpCls();
  res += testDimlpPred();
  res += testDimlpRul();

  return res;
}

// test avec dimlpTrn weights file
int runFidextestsSuite() {
  int res = 0;

  res += testFidex();
  res += testFidexGloRules();
  res += testFidexGlo();
  res += testFidexGloStats();

  return res;
}


int main() {
  int res = 0;
  std::cout << "Running tests... (can take a while to finish)" << std::endl;

  res += runDimlpTestsSuite();
  res += runFidextestsSuite();
  res += runCommonTestSuite();

  std::cout << std::to_string(res) << " tests done." << std::endl;
  return 0;
}
