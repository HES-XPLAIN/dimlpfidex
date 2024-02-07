#include "../headers/parametersTests.hpp"

void testSetter() {
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

  try {
    p.setString(TEST_DATA_FILE, "../../dataset/data/fake_test.txt");
    p.sanitizePath(TEST_DATA_FILE, true);
    testAssert("Parameter throw on invalid file path", false);

  } catch (ErrorHandler &e) {
    std::string expectedMessage = getInvalidFileOrDirectoryMessage(TEST_DATA_FILE, "../../dataset/data/fake_test.txt");
    std::string actualMessage = e.what();

    testAssert("Parameter throw on invalid file path", expectedMessage.compare(actualMessage) == 0);
  }

  p.setString(ATTRIBUTES_FILE, DEFAULT_ATTRIBUTES_FILE);
  p.sanitizePath(ATTRIBUTES_FILE, true);
  testAssert("Parameter sanitize path of input file", true);

  p.setString(RULES_OUTFILE, DEFAULT_TXT_OUT_RULES_FILE);
  p.sanitizePath(RULES_OUTFILE, false);
  testAssert("Parameter sanitize path of output file", true);

}

void testGetter() {
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
    p.getString(HEURISTIC);
    testAssert("Parameter get exception on wrong value", false);
  } catch (ErrorHandler &e) {
    std::string expectedMessage = getArgumentNotFoundExceptionMessage(HEURISTIC);
    std::string actualMessage = e.what();
    testAssert("Parameter get exception on wrong value", expectedMessage.compare(actualMessage) == 0);
  }

  try {
    p.getString(TRAIN_DATA_FILE);
    testAssert("Parameter throw on empty getter", false);
  } catch (ErrorHandler &e) {
    std::string expectedMessage = getArgumentNotFoundExceptionMessage(TRAIN_DATA_FILE);
    std::string actualMessage = e.what();

    testAssert("Parameter throw on empty getter", expectedMessage.compare(actualMessage) == 0);
  }
}

void testDefaultSetter() {
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

void testArgsParser() {
  vector<string> args = {
      "executableNameToIgnore", // this line has to be present
      "--train_data_file", DEFAULT_TRAIN_FILE,
      "--heuristic", "1",
      "--dropout_hyp", "0.4"};

  Parameters p = Parameters(args);

  testAssert("Parameter by user args: parse int", p.getInt(HEURISTIC) == 1);
  testAssert("Parameter by user args: parse string", p.getString(TRAIN_DATA_FILE).compare(DEFAULT_TRAIN_FILE) == 0);
  testAssert("Parameter by user args: parse float", p.getFloat(DROPOUT_HYP) == 0.4f);

  try {
    vector<string> args = {
        "executableNameToIgnore", // this line has to be present
        "--nb_threads", "1.5"};

    Parameters p = Parameters(args);

  } catch (ErrorHandler &e) {
    std::string expectedMessage = getInvalidDataTypeExceptionMessage(NB_THREADS, "1.5", "integer");
    std::string actualMessage = e.what();

    testAssert("Parameter throw on float instead of int", expectedMessage.compare(actualMessage) == 0);
  }
}

void testJsonParser() {
  vector<string> args = {
      "executableNameToIgnore",
      "--train_data_file", DEFAULT_TRAIN_FILE,
      "--train_pred_file", DEFAULT_TRAIN_PRED_FILE,
      "--train_class_file", DEFAULT_TRAIN_TRUE_CLASS_FILE,
      "--global_rules_outfile", DEFAULT_TXT_OUT_RULES_FILE,
      "--attributes_file", DEFAULT_ATTRIBUTES_FILE,
      "--weights_file", DEFAULT_WEIGHTS_FILE,
      "--nb_attributes", "31",
      "--nb_classes", "7",
      "--nb_dimlp_nets", "1",
      "--nb_quant_levels", "50",
      "--heuristic", "1",
      "--max_iterations", "100",
      "--min_covering", "2",
      "--max_failed_attempts", "10",
      "--nb_threads", "4",
      "--positive_class_index", "-1",
      "--seed", "0",
      "--decision_threshold", "-1.0",
      "--dropout_hyp", "0.3",
      "--dropout_dim", "0.3",
      "--min_fidelity", "1.0"};

  std::stringstream buffer1;
  std::stringstream buffer2;

  Parameters pJson = Parameters("fidexTests/templates/default_config.json");
  Parameters pArgs = Parameters(args);

  testAssert("Parameters: JSON & user args are equal", buffer1.str().compare(buffer2.str()) == 0);
}