#include "../headers/dimlpTests.hpp"

uint testDensCls() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_densCls.json";
  std::string testName = "Simple densCls execution";

  return testExec(testName, command, densCls);
}

uint testDimlpBT() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpBT.json";
  std::string testName = "Simple dimlpBT execution";

  return testExec(testName, command, dimlpBT);
}

uint testDimlpCls() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpCls.json";
  std::string testName = "Simple dimlpCls execution";

  return testExec(testName, command, dimlpCls);
}

uint testDimlpPred() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpPred.json";
  std::string testName = "Simple dimlpPred execution";

  return testExec(testName, command, dimlpPred);
}

uint testDimlpRul() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpRul.json";
  std::string testName = "Simple dimlpRul execution";

  return testExec(testName, command, dimlpRul);
}

uint testDimlpTrn() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpTrn.json";
  std::string testName = "Simple dimlpTrn execution";

    return testExec(testName, command, dimlpTrn);
}
