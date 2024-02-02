// #include "../headers/fidexGloTests.hpp"

// void testHelpOutput() {
//   map<string, string> args = {};
//   string argsStr = argsToString(args);

//   testAssert("Help output", fidexGloRules(argsStr) != 0);
// }

// // TODO continue here
// void testMissingParameter(const std::string &parameterName) {
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

// // void test_with_user_args() {
// //   auto p = Parameters(DEFAULT_JSON_CONFIG_FILE);
// // }

// // void test_with_json_file() {
// //   auto p = Parameters(DEFAULT_JSON_CONFIG_FILE);
// // }


