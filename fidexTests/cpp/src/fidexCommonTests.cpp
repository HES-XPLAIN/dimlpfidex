#include "../headers/fidexCommonTests.hpp"

std::string getInvalidDataTypeExceptionMessage(ParameterCode id, const string &wrongValue, const string &typeName) {
  return "CommandArgumentException: Parsing error: argument (ID " + Parameters::getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid " + typeName + ".";
}

std::string getAlreadySetArgumentExceptionMessage(ParameterCode id, const string &value) {
  return "CommandArgumentException: Parsing error: argument (ID " + Parameters::getParameterName(id) + ") with value \"" + value + "\" is already set, cannot override it.";
}

std::string getArgumentNotFoundExceptionMessage(ParameterCode id) {
  return "CommandArgumentException: Parameters error: argument (ID " + Parameters::getParameterName(id) + ") requested was not found, try to rerun including it.";
}

void test_parameters_setter() {
  Parameters p;

  p.setInt(HEURISTIC, 1);
  testAssert("Parameter is set", p.isIntSet(HEURISTIC));
  testAssert("Parameter is not set", !p.isIntSet(NB_THREADS));

  try {
    // this must throw an exception
    p.setInt(HEURISTIC, 10);
    testAssert("Parameter throw on double set", false);
  } catch (ErrorHandler &e) {
    std::string expectedMessage = getAlreadySetArgumentExceptionMessage(HEURISTIC, "10");
    std::string actualMessage = e.what();

    testAssert("Parameter throw on double set", expectedMessage.compare(actualMessage) == 0);
  }
}

void test_parameters_getter() {
  Parameters p;
  p.setString(ROOT_FOLDER, "folder/");
  p.setInt(HEURISTIC, 1);
  p.setFloat(DROPOUT_DIM, 0.5f);
  p.setDouble(DROPOUT_HYP, 0.99999);

  testAssert("Parameter get correct string value", p.getString(ROOT_FOLDER).compare("folder/") == 0);
  testAssert("Parameter get correct int value", p.getInt(HEURISTIC) == 1);
  testAssert("Parameter get correct float value", p.getFloat(DROPOUT_DIM) == 0.5f);
  testAssert("Parameter get correct double value", p.getDouble(DROPOUT_HYP) == 0.99999);

  try {
    p.getString(TRAIN_DATA_FILE);
    testAssert("Parameter throw on empty getter", false);
  } catch (ErrorHandler &e) {
    std::string expectedMessage = getArgumentNotFoundExceptionMessage(TRAIN_DATA_FILE);
    std::string actualMessage = e.what();

    testAssert("Parameter throw on empty getter", expectedMessage.compare(actualMessage) == 0);
  }
}

void test_parameters_default_setter() {
  Parameters p;
  std::string value = "somewhere/over/the/rainbow/";
  std::string new_value = "/tmp/";

  p.setDefaultString(ROOT_FOLDER, value);
  testAssert("Parameter is default value set", p.isStringSet(ROOT_FOLDER));
  testAssert("Parameter get default value", value.compare(p.getString(ROOT_FOLDER)) == 0);

  p.setString(TEST_DATA_FILE, value);
  p.setDefaultString(TEST_DATA_FILE, new_value);
  testAssert("Parameter value not overrided", new_value.compare(p.getString(TEST_DATA_FILE)) != 0);
}

void test_parameters_arg_parser() {
  vector<string> args = {
      "executable to be ignored", // this line has to be present
      "--train_data_file", "the/rainbow/",
      "--heuristic", "1",
      "--dropout_hyp", "0.4"};

  Parameters p = Parameters(args);

  testAssert("Parameter by user args: parse int", p.getInt(HEURISTIC) == 1);
  testAssert("Parameter by user args: parse string", p.getString(TRAIN_DATA_FILE).compare("the/rainbow/") == 0);
  testAssert("Parameter by user args: parse float", p.getFloat(DROPOUT_HYP) == 0.4f);

  args.push_back("--root_folder"); 
  args.push_back("somehere/over/");
  testAssert("Parameter by user args: root_folder concat", p.getString(TRAIN_DATA_FILE).compare("somehere/over/the/rainbow/"));
}

// void test_parameters_json_parser();

void run_fidexcommon_test_suite() {
  test_parameters_setter();
  test_parameters_getter();
  test_parameters_default_setter();
  test_parameters_arg_parser();
}