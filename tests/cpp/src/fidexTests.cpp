#include "../headers/fidexTests.hpp"

void testFidex() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidex.json";
  std::string testName = "Simple fidex execution";

  try {
    int code = fidex(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}

void testFidexGlo() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGlo.json";
  std::string testName = "Simple fidexGlo execution";

  try {
    int code = fidexGlo(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}

void testFidexGloRules() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGloRules.json";
  std::string testName = "Simple fidexGloRules execution";

  try {
    int code = fidexGloRules(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}

void testFidexGloStats() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGloStats.json";
  std::string testName = "Simple fidexGloStats execution";

  try {
    int code = fidexGloStats(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}