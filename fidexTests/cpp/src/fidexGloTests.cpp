#include "../headers/fidexGloTests.hpp"

void testFidexGloRules() {
  std::map<std::string, std::string> args = DEFAULT_ARGS;
  args.insert({"--console_file", DEFAULT_CONSOLE_FILE});

  std::string strArgs = argsToString(args);

  try {
    int code = fidexGloRules(strArgs);
    testAssert("Simple fidexGloRules execution", code == 0);
  } catch (std::exception &e) {
    testAssert("Simple fidexGloRules execution", false);
  }
}
