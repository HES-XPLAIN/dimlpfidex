#include "../headers/fidexGloTests.hpp"

// void test_help_output() {
//   map<string, string> args = {};
//   string argsStr = argsToString(args);

//   testAssert("Help output", fidexGloRules(argsStr) != 0);
// }

// // TODO continue here
// void test_missing_parameter(const std::string &parameterName) {
//   map<string, string> args = remove(DEFAULT_ARGS, {"--data_train_file"});
//   string argsStr = argsToString(args);

//   try {
//     fidexGloRules(argsStr);
//     testAssert("Train data file missing", false);

//   } catch (CommandArgumentException &e) {
//     std::string expectedException = "Parameters error: argument (ID " + parameterName + ") requested was not found, try to rerun including it.";
//     std::string actualException(e.what());
//     int diff = expectedException.compare(actualException);

//     testAssert("Train data file missing", diff == 0);
//   }
// }

// void test_parameters_from_json_file() {
//   auto p = Parameters(DEFAULT_JSON_CONFIG_FILE);
// }


