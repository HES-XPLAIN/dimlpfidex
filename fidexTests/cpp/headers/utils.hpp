#ifndef UTILS_H
#define UTILS_H

#include "../../../common/cpp/src/parameters.h"
#include "../../../json/single_include/nlohmann/json.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using Json = nlohmann::json;

// default values to avoid generating litterals redundancy
static const std::string DATA_FOLDER = "fidexTests/dataset/data/";

// default paths
static const std::string DEFAULT_TRAIN_FILE = DATA_FOLDER + "train.txt";
static const std::string DEFAULT_TRAIN_PRED_FILE = DATA_FOLDER + "train.out";
static const std::string DEFAULT_TRAIN_TRUE_CLASS_FILE = DATA_FOLDER + "train_true_classes.txt";
static const std::string DEFAULT_WEIGHTS_FILE = DATA_FOLDER + "weights.wts";
static const std::string DEFAULT_TXT_IN_RULES_FILE = DATA_FOLDER + "rules.txt";
static const std::string DEFAULT_TXT_OUT_RULES_FILE = DATA_FOLDER + "new_rules.txt";
static const std::string DEFAULT_JSON_IN_RULES_FILE = DATA_FOLDER + "rules.json";
static const std::string DEFAULT_JSON_OUT_RULES_FILE = DATA_FOLDER + "new_rules.json";
static const std::string DEFAULT_ATTRIBUTES_FILE = DATA_FOLDER + "attributes.txt";
static const std::string DEFAULT_CONSOLE_FILE = DATA_FOLDER + "console.out";

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

// printing utils
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

const std::map<std::string, std::string> DEFAULT_ARGS{
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

void testAssert(const std::string &testName, bool condition);

// specific helpers to handle Parameters tests use cases
std::string argsToString(const std::map<std::string, std::string> &args);
std::map<std::string, std::string> remove(const std::map<std::string, std::string> &args, const std::vector<std::string> &toRemove);

// custom exception messages
std::string getArgumentNotFoundExceptionMessage(ParameterCode id);
std::string getAlreadySetArgumentExceptionMessage(ParameterCode id, const std::string &value);
std::string getInvalidFileOrDirectoryMessage(ParameterCode id, const std::string &wrongValue);
std::string getInvalidDataTypeExceptionMessage(ParameterCode id, const std::string &wrongValue, const std::string &typeName);

#endif // UTILS_H
