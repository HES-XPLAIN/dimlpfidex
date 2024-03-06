#include "../headers/dimlpTests.hpp"


void testDensCls() {
<<<<<<< HEAD
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_densCls.json";
  std::string testName = "Simple densCls execution";

  try {
    int code = densCls(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
=======
  std::cout << "testDensCls() not implemented." << std::endl;
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_densCls.json";
  densCls(command);
>>>>>>> 6adbcd3 (Refactored test folder)
}

void testDimlpBT() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpBT.json";
  std::string testName = "Simple dimlpBT execution";

  try {
    int code = dimlpBT(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}

void testDimlpCls() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpCls.json";
  std::string testName = "Simple dimlpCls execution";

  try {
    int code = dimlpCls(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}

void testDimlpPred() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpPred.json";
  std::string testName = "Simple dimlpPred execution";

  try {
    int code = dimlpPred(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}

void testDimlpRul() {
  std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpRul.json";
  std::string testName = "Simple dimlpRul execution";

  try {
    int code = dimlpRul(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}

void testDimlpTrn() {
    std::string command = "--json_config_file " + TEMPLATES_PATH + "config_dimlpTrn.json";
  std::string testName = "Simple dimlpTrn execution";

  try {
    int code = dimlpTrn(command);
    testAssert(testName, code == 0);
  } catch (exception &e) {
    testAssert(testName, false);
  }
}
