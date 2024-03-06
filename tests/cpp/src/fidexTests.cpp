#include "../headers/fidexTests.hpp"

void testFidex() {
  std::cout << "testFidex() not implemented." << std::endl;
}

void testFidexGlo() {
  std::cout << "testFidexGlo() not implemented." << std::endl;
}

void testFidexGloRules() {
  map<std::string, std::string> args = DEFAULT_ARGS;
  args.insert({"--console_file", DEFAULT_CONSOLE_FILE});
  
  std::string strArgs = argsToString(args);

  try {
    int code = fidexGloRules(strArgs);
    testAssert("Simple fidexGloRules execution", code == 0);
  } catch (exception &e) {
    testAssert("Simple fidexGloRules execution", false);
  }
}

void testFidexGloStats() {
  std::cout << "testFidexGloStats() not implemented." << std::endl;
}