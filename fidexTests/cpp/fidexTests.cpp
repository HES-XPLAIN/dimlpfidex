#include "headers/fidexTests.hpp"

void runFidextestsSuite() {
  runFidexcommonTestSuite();
  testFidexGloRules();
}

int main() {
  runFidextestsSuite();
  return 0;
}
