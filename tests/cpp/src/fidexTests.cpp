#include "../headers/fidexTests.hpp"

uint testFidex() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidex.json";
  std::string testName = "Simple fidex execution";

  return testExec(testName, command, fidex);
}

uint testFidexGlo() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGlo.json";
  std::string testName = "Simple fidexGlo execution";

  return testExec(testName, command, fidexGlo);
}

uint testFidexGloRules() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGloRules.json";
  std::string testName = "Simple fidexGloRules execution";

  return testExec(testName, command, fidexGloRules);
}

uint testFidexGloStats() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGloStats.json";
  std::string testName = "Simple fidexGloStats execution";

  return testExec(testName, command, fidexGloStats);
}