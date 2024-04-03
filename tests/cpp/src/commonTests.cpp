#include "../headers/commonTests.hpp"

uint runParametersTestSuite() {
  uint res = 0;
  
  res += testSetter();
  res += testGetter();
  res += testDefaultSetter();
  res += testArgsParser();
  res += testJsonParser();

  return res;
}

// main function calling all classes tests
uint runCommonTestSuite() {
  return runParametersTestSuite();
}
