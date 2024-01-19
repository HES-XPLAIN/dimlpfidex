#ifndef TESTS_H
#define TESTS_H

// -------------------------------------------------
// Obligatory parameters :
// -T <train data file>
// -P <train prediction file>
// -C <train true class file, not mendatory if classes are specified in train data file>
// -W <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with -N> [Not mendatory if a rules file is given with -f]
// -f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given]
// -O <Rules output file>
// -M <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo>
// -a <number of attributes>
// -b <number of classes><Options>
//
// Options are:
// -S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>
// -N <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>
// -A <file of attributes>
// -r <file where you redirect console result>
// -i <max iteration number (100 by default)>
// -v <minimum covering number (2 by default)>
// -d <dimension dropout parameter>
// -h <hyperplan dropout parameter>
// -m <maximum number of failed attempts to find Fidex rule when covering is 1 (30 by default)>
// -Q <number of stairs in staircase activation function (50 by default)>
// -t <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>
// -x <index of positive class for the usage of decision threshold (None by default, 0 for first one)>
// -p <number of threads used for computing the algorithm (default=1, this means by default its a sequential execution)>
// -y <minimal rule fidelity accepted when generating a rule [0,1] (default=1)>
// -z <seed (0=random, default)>
// -------------------------------------------------

#include "../src/fidexGloRulesFct.h"

// define default values to avoid litterals redundancy
#define PROGRAM_NAME "fidexGloRules"
#define DATA_FOLDER "./dataset/data/"
#define OUT_FOLDER "./dataset/out/"

// default paths
#define TRAIN_FILE DATA_FOLDER "train.txt"
#define TRAIN_PRED_FILE DATA_FOLDER "train.out"
#define TRAIN_TRUE_CLASS_FILE DATA_FOLDER "train_true_classes.txt"
#define WEIGHTS_FILE DATA_FOLDER "weights.txt"
#define JSON_CONFIG_FILE DATA_FOLDER "config.json"
#define TXT_IN_RULES_FILE DATA_FOLDER "rules.txt"
#define TXT_OUT_RULES_FILE OUT_FOLDER "new_rules.txt"
#define JSON_IN_RULES_FILE DATA_FOLDER "rules.json"
#define JSON_OUT_RULES_FILE OUT_FOLDER "new_rules.json"
#define ATTRIBUTES_FILE DATA_FOLDER "attributes.txt"
#define CONSOLE_FILE OUT_FOLDER "console.out"

// numeric args
#define HEURISTIC "1"
#define NB_ATTRIBUTES "31"
#define NB_CLASSES "7"
#define NB_BAGGING_NETS "1"
#define MAX_ITERATIONS "100"
#define MIN_COVERING "2"
#define DROPOUT_DIM "0.3"
#define DROPOUT_HYP "0.3"
#define MAX_FAILED_ATTEMPTS "10"
#define NB_STAIRS_STAIRCASE_FN "50"
#define DECISION_THRESHOLD "-1.0"
#define IDX_POSITIVE_CLASS "-1.0"
#define NB_THREADS "4"
#define MIN_FIDELITY "1.0"
#define SEED "0"

using namespace std;

string formatArg(string flag, string value) {
  return flag + " " + value + " ";
}

string getArgs(const vector<string> &v) {
  stringstream ss;
  for (string e : v) {
    ss << e << " ";
  }

  return ss.str();
}

const vector<string> defaultArgs{
    formatArg("-T", TRAIN_FILE),
    formatArg("-P", TRAIN_PRED_FILE),
    formatArg("-C", TRAIN_TRUE_CLASS_FILE),
    formatArg("-W", WEIGHTS_FILE),
    // formatArg("-f", TXT_IN_RULES_FILE), // TODO
    formatArg("-O", TXT_OUT_RULES_FILE),
    formatArg("-M", HEURISTIC),
    formatArg("-a", NB_ATTRIBUTES),
    formatArg("-b", NB_CLASSES),
    // formatArg("-S", DATA_FOLDER), // TODO
    formatArg("-N", NB_BAGGING_NETS),
    formatArg("-A", ATTRIBUTES_FILE),
    formatArg("-r", CONSOLE_FILE),
    formatArg("-i", MAX_ITERATIONS),
    formatArg("-v", MIN_COVERING),
    formatArg("-d", DROPOUT_DIM),
    formatArg("-h", DROPOUT_HYP),
    formatArg("-m", MAX_FAILED_ATTEMPTS),
    formatArg("-Q", NB_STAIRS_STAIRCASE_FN),
    formatArg("-t", DECISION_THRESHOLD),
    formatArg("-x", IDX_POSITIVE_CLASS),
    formatArg("-p", NB_THREADS),
    formatArg("-y", MIN_FIDELITY),
    formatArg("-z", SEED)};

#endif // TESTS_H
