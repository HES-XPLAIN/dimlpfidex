#include "../headers/fidexTests.hpp"

int testFidex() {
  int res = 0;

  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidex_1.json";
  std::string testName = "Simple fidex execution (with dimlpTrn weights)";
  res += testExec(testName, command, fidex);

  command = "--json_config_file " + TEMPLATES_PATH + "config_fidex_2.json";
  testName = "Simple fidex execution (with dimlpBT weights)";
  res += testExec(testName, command, fidex);

  return res;
}

int testFidexGlo() {
  int res = 0;

  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGlo_1.json";
  std::string testName = "Simple fidexGlo execution (with dimlpTrn weights)";
  res += testExec(testName, command, fidexGlo);

  command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGlo_2.json";
  testName = "Simple fidexGlo execution (with dimlpBT weights)";
  res += testExec(testName, command, fidexGlo);

  return res;
}

int testFidexGloRules() {
  int res = 0;

  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGloRules_1.json";
  std::string testName = "Simple fidexGloRules execution (with dimlpTrn weights)";
  res += testExec(testName, command, fidexGloRules);

  command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGloRules_2.json";
  testName = "Simple fidexGloRules execution (with dimlpBT weights)";
  res += testExec(testName, command, fidexGloRules);

  return res;
}

int testFidexGloStats() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_fidexGloStats.json";
  std::string testName = "Simple fidexGloStats execution";

  return testExec(testName, command, fidexGloStats);
}