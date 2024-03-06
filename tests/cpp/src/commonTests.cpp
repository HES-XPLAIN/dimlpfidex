#include "../headers/commonTests.hpp"

void runParametersTestSuite() {
  testSetter();
  testGetter();
  testDefaultSetter();
  testArgsParser();
  testJsonParser();
}

// main function calling all classes tests
void runCommonTestSuite() {
  runParametersTestSuite();
}
