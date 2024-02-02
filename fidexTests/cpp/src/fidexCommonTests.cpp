#include "../headers/fidexCommonTests.hpp"


void runParametersTestSuite() {
  testSetter();
  testGetter();
  testDefaultSetter();
  testArgsParser();
  testJsonParser();
}

// main function calling all classes tests
void runFidexcommonTestSuite() {
  runParametersTestSuite();
}