#ifndef TESTS_H
#define TESTS_H

#include "../../../fidexCommon/cpp/src/parameters.h"
#include "../src/fidexGloRulesFct.h"
#include <map>

// printing utils
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

// default values to avoid generating litterals redundancy
static const std::string PROGRAM_NAME = "fidexGloRules";
static const std::string DATA_FOLDER = "./dataset/data/";
static const std::string OUT_FOLDER = "./dataset/out/";
// TODO: find a way to include relative path instead
static const std::string DEFAULT_JSON_CONFIG_FILE = "/home/eldado/Documents/hepia/dimlpfidex-json/dimlpfidex/fidexGlo/cpp/tests/config.json";

// default paths
static const std::string DEFAULT_TRAIN_FILE = DATA_FOLDER + "train.txt";
static const std::string DEFAULT_TRAIN_PRED_FILE = DATA_FOLDER + "train.out";
static const std::string DEFAULT_TRAIN_TRUE_CLASS_FILE = DATA_FOLDER + "train_true_classes.txt";
static const std::string DEFAULT_WEIGHTS_FILE = DATA_FOLDER + "weights.txt";
static const std::string DEFAULT_TXT_IN_RULES_FILE = DATA_FOLDER + "rules.txt";
static const std::string DEFAULT_TXT_OUT_RULES_FILE = OUT_FOLDER + "new_rules.txt";
static const std::string DEFAULT_JSON_IN_RULES_FILE = DATA_FOLDER + "rules.json";
static const std::string DEFAULT_JSON_OUT_RULES_FILE = OUT_FOLDER + "new_rules.json";
static const std::string DEFAULT_ATTRIBUTES_FILE = DATA_FOLDER + "attributes.txt";
static const std::string DEFAULT_CONSOLE_FILE = OUT_FOLDER + "console.out";

// default numeric args
static const std::string DEFAULT_HEURISTIC = "1";
static const std::string DEFAULT_NB_ATTRIBUTES = "31";
static const std::string DEFAULT_NB_CLASSES = "7";
static const std::string DEFAULT_NB_BAGGING_NETS = "1";
static const std::string DEFAULT_MAX_ITERATIONS = "100";
static const std::string DEFAULT_MIN_COVERING = "2";
static const std::string DEFAULT_DROPOUT_DIM = "0.3";
static const std::string DEFAULT_DROPOUT_HYP = "0.3";
static const std::string DEFAULT_MAX_FAILED_ATTEMPTS = "10";
static const std::string DEFAULT_NB_STAIRS_STAIRCASE_FN = "50";
static const std::string DEFAULT_DECISION_THRESHOLD = "-1.0";
static const std::string DEFAULT_IDX_POSITIVE_CLASS = "-1";
static const std::string DEFAULT_NB_THREADS = "4";
static const std::string DEFAULT_MIN_FIDELITY = "1.0";
static const std::string DEFAULT_SEED = "0";

std::string argsToString(const map<std::string, std::string> &args) {
  std::stringstream ss;
  for (const auto &kv : args) {
    ss << kv.first << " " << kv.second << " ";
  }

  return ss.str();
}

map<std::string, std::string> remove(const map<std::string, std::string> &args, const vector<std::string> &toRemove) {
  map<std::string, std::string> result = args;

  for (std::string item : toRemove) {
    result.erase(item);
  }

  return result;
}

void testAssert(const std::string &testName, bool condition) {
  if (condition) {
    cout << GREEN << "[" << testName << "] "
         << "passed" << RESET << endl;
  } else {
    cout << RED << "[" << testName << "] "
         << "failed" << RESET << endl;
  }
}

const map<std::string, std::string> DEFAULT_ARGS{
    {"--train_data_file", DEFAULT_TRAIN_FILE},
    {"--train_pred_file", DEFAULT_TRAIN_PRED_FILE},
    {"--train_class_file", DEFAULT_TRAIN_TRUE_CLASS_FILE},
    // {"--rules_file", DEFAULT_TXT_IN_RULES_FILE}, // TODO
    {"--rules_outfile", DEFAULT_TXT_OUT_RULES_FILE},
    {"--console_file", DEFAULT_CONSOLE_FILE},
    // {"--root_folder", DEFAULT_ROOT_FOLDER}, // TODO
    {"--attributes_file", DEFAULT_ATTRIBUTES_FILE},
    {"--weights_file", DEFAULT_WEIGHTS_FILE},
    {"--nb_attributes", DEFAULT_NB_ATTRIBUTES},
    {"--nb_classes", DEFAULT_NB_CLASSES},
    {"--nb_dimlp_nets", DEFAULT_NB_BAGGING_NETS},
    {"--nb_quant_levels", DEFAULT_NB_STAIRS_STAIRCASE_FN},
    {"--heuristic", DEFAULT_HEURISTIC},
    {"--max_iterations", DEFAULT_MAX_ITERATIONS},
    {"--min_covering", DEFAULT_MIN_COVERING},
    {"--max_failed_attempts", DEFAULT_MAX_FAILED_ATTEMPTS},
    {"--nb_threads", DEFAULT_NB_THREADS},
    {"--positive_class_index", DEFAULT_IDX_POSITIVE_CLASS},
    {"--seed", DEFAULT_SEED},
    {"--decision_threshold", DEFAULT_DECISION_THRESHOLD},
    // {"--hi_knot", HI_KNOT}, // TODO
    {"--dropout_hyp", DEFAULT_DROPOUT_DIM},
    {"--dropout_dim", DEFAULT_DROPOUT_HYP},
    {"--min_fidelity", DEFAULT_MIN_FIDELITY}};

#endif // TESTS_H
