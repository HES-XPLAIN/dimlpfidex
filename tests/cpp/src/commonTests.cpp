#include "../headers/commonTests.hpp"

int runParametersTestSuite() {
  int res = 0;
  
  res += testSetter();
  res += testGetter();
  res += testDefaultSetter();
  res += testArgsParser();
  res += testJsonParser();

  return res;
}

// main function calling all classes tests
int runCommonTestSuite() {
  return runParametersTestSuite();
}
