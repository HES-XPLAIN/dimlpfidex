#include "../headers/dimlpTests.hpp"

int testDensCls() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_densCls.json";
  std::string testName = "Simple densCls execution";

  return testExec(testName, command, densCls);
}

int testDimlpBT() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpBT.json";
  std::string testName = "Simple dimlpBT execution";

  return testExec(testName, command, dimlpBT);
}

int testDimlpCls() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpCls.json";
  std::string testName = "Simple dimlpCls execution";

  return testExec(testName, command, dimlpCls);
}

int testDimlpPred() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpPred.json";
  std::string testName = "Simple dimlpPred execution";

  return testExec(testName, command, dimlpPred);
}

int testDimlpRul() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpRul.json";
  std::string testName = "Simple dimlpRul execution";

  return testExec(testName, command, dimlpRul);
}

int testDimlpTrn() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpTrn.json";
  std::string testName = "Simple dimlpTrn execution";

    return testExec(testName, command, dimlpTrn);
}
