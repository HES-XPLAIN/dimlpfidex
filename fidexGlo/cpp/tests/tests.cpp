#include "tests.h"

void test_help_output() {
  map<string, string> args = {};
  string argsStr = argsToString(args);

  testAssert("Help output", fidexGloRules(argsStr) != 0);
}

void test_train_data_file() {
  map<string, string> args = remove(DEFAULT_ARGS, {"-T"});
  string argsStr = argsToString(args);

  try {
    fidexGloRules(argsStr);
    testAssert("Train data file missing", false);
  } catch (CommandArgumentException e) {
    testAssert("Train data file missing", true);
    cout << e.what() << endl;
  }
}

// TODO continue here

int main(int argc, char **argv) {
  test_help_output();
  test_train_data_file();

  return EXIT_SUCCESS;
}
