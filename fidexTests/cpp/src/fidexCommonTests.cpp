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
  p.setFloat(DROPOUT_DIM, 0.5f);
  testAssert("Parameter get value", p.getFloat(DROPOUT_DIM) == 0.5f);

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
  p.setDefaultString(ROOT_FOLDER, value);
  testAssert("Parameter is default value set", p.isStringSet(ROOT_FOLDER));
  testAssert("Parameter get default value", value.compare(p.getString(ROOT_FOLDER)) == 0);

  std::string new_value = "/tmp/";
  p.setString(ROOT_FOLDER, new_value);
  testAssert("Parameter override default value", new_value.compare(p.getString(ROOT_FOLDER)) == 0);
}

// void test_parameters_arg_parser();
// void test_parameters_json_parser();

void run_fidexcommon_test_suite() {
  test_parameters_setter();
  test_parameters_getter();
  test_parameters_default_setter();
}