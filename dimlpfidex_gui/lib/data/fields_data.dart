import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/data/common_fields_data.dart';

// TODO Handle differenciation between float and int
// TODO: handle this input to fit float & int options

// const List<Field> testFields = [
//   rootFolderFld,
//   Field(
//     "Normalization file",
//     "normalization_file",
//     Datatype.filePath,
//     defaultValue: "normalization.txt",
//   ),
//   seedFld,
//   dropoutDimFld,
//   Field(
//     "Data files to normalize",
//     "data_files",
//     Datatype.listFilePath,
//     description:
//         "Data files to normalize, they are normalized with respect to the first one if normalization_file is not specified",
//   ),
//   Field("Dropdown", "dropdown", Datatype.restrictedChoiceString,
//       description: "Random dropdown",
//       items: [
//         "choice one",
//         "choice two",
//         "choice three",
//         "choice four",
//         "choice five"
//       ]),
//   Field(
//     "Using dichotomic search",
//     "search-strategy",
//     Datatype.boolean,
//     description:
//         "Whether or not to use the dichotomic search strategy, can increase speed.",
//     defaultValue: "true",
//   ),
//   musFld,
//   sigmasFld,
//   Field(
//     "Enums",
//     "enums",
//     Datatype.listString,
//     description: "List of words",
//     defaultValue: "hello, my, name, is, test",
//   ),
//   Field(
//     "Random dict",
//     "random_dict",
//     Datatype.dictionary,
//     description: "Dictionary",
//     defaultValue: "{1:2, 3:4, 5:6}",
//   ),
// ];

const List<Field> fidexFields = [
  rootFolderFld,
  trainDataFileFld,
  trainPredFileFld,
  Field(
    "Train true classes file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train true classes file, not mandatory if classes are specified in train data file.",
  ),
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    isRequired: true,
    description:
        "Test sample(s) data file with data, prediction (if no Test prediction file is specified) and true class (if no Test true classes file is specified).",
  ),
  Field(
    "Weights file",
    "weights_file",
    Datatype.filePath,
    description:
        "Weights file (not mandatory if a rules file is given with Rules file).",
  ),
  Field(
    "Rules file",
    "rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with weights file).",
  ),
  rulesOutputFileFld,
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Test prediction file",
    "test_pred_file",
    Datatype.filePath,
    description:
        "Path/name to file containing predictions concerning the test portion of a dataset.",
  ),
  Field(
    "Test true classes file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test true class file. If at least Test prediction file is specified, Test data file needs to have only test datas and eventually classes on same line (don't add Test true classes file in this case).",
  ),
  Field(
    "Attributes file",
    "attributes_file",
    Datatype.filePath,
    description: "File containing labels of attributes and classes.",
  ),
  Field(
    "Output stats file",
    "stats_file",
    Datatype.filePath,
    description:
        "Name/path of file generated containing information concerning the algorithm execution.",
  ),
  consoleFileFld,
  Field(
    "Maximum iteration number",
    "max_iterations",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "10",
    description:
        "Maximum iteration number, also the maximum possible number of attributes in a rule, should be 25 if working with images.",
  ),
  Field(
    "Minimum covering number",
    "min_covering",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "2",
    description: "Minimum number of samples covered by the generated rules.",
  ),
  Field(
    "Use covering strategy",
    "covering_strategy",
    Datatype.boolean,
    defaultValue: "true",
    description:
        "Whether to use the covering strategy: if no rule is found with minimum covering, find best rule with best covering using dichotomic search. Decreases minimum fidelity if needed.",
  ),
  Field(
    "Maximum number of failed attempts",
    "max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "30",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used.",
  ),
  Field(
    "Minimal rule fidelity accepted",
    "min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "1.0",
    description: "Minimal rule fidelity accepted when generating a rule.",
  ),
  Field(
    "Lowest minimal rule fidelity accepted",
    "lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "0.75",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy.",
  ),
  dropoutDimFld,
  dropoutHypFld,
  nbQuantLevelsFld,
  Field(
    "Decision threshold for predictions",
    "decision_threshold",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    description:
        "Decision threshold for predictions, you need to specify the index of positive class if you want to use it.",
  ),
  Field(
    "Index of positive class",
    "positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "# classes - 1",
    description:
        "Index of positive class for the usage of decision threshold, index starts at 0.",
  ),
  Field(
    "File containing the mean and standard deviation",
    "normalization_file",
    Datatype.filePath,
    description:
        "File containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld,
];

const List<Field> fidexGloFields = [
  rootFolderFld,
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description:
        "Test sample(s) data file with data and prediction. If no test prediction file is specified, classes may be included here if launching with fidex ('with fidex' option).",
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Attributes file",
    "attributes_file",
    Datatype.filePath,
    description:
        "File of attributes. Mandatory if rules file contains attribute names, if not, do not add it.",
  ),
  Field(
    "Test prediction file",
    "test_pred_file",
    Datatype.filePath,
    description:
        "Test prediction file. If given, test data file needs to have only test data.",
  ),
  Field(
    "Explanation file",
    "explanation_file",
    Datatype.filePath,
    description: "Explanation(s) output file name.",
  ),
  consoleFileFld,
  Field(
    "Decision threshold",
    "decision_threshold",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Decision threshold for predictions. You need to specify the index of the positive class if you want to use it.",
  ),
  Field(
    "Index of positive class",
    "positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "# classes - 1",
    description:
        "Index of positive class for the usage of decision threshold. Index starts at 0.",
  ),
  Field(
    "Use fidex algorithm",
    "with_fidex",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to use Fidex algorithm if no rule is found in global rules.",
  ),
  Field(
    "Use minimal version",
    "with_minimal_version",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to use minimal version, which only gets correct activated rules and if with_fidex, launches Fidex when no such rule is found.",
  ),
  trainDataFileFld,
  trainPredFileFld,
  Field(
    "Train true class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train true class file. Not mandatory if classes are specified in train data file.",
  ),
  Field(
    "Weights file",
    "weights_file",
    Datatype.filePath,
    description:
        "Weights file. Not mandatory if a rules file is given with 'rules file'.",
  ),
  Field(
    "Rules file",
    "rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus. Not mandatory if a weights file is given with 'weights file'.",
  ),
  Field(
    "Test true classes file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test true classes file. Classes can be specified in the test data file.",
  ),
  Field(
    "Max iteration number",
    "max_iterations",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "10",
    description:
        "Max iteration number, also the max possible number of attributes in a rule. Should be 25 if working with images.",
  ),
  Field(
    "Minimum covering number",
    "min_covering",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "2",
    description: "Minimum covering number.",
  ),
  Field(
    "Use covering stategy",
    "covering_strategy",
    Datatype.boolean,
    defaultValue: "true",
    description:
        "Whether to use this strategy: if no rule is found with min_covering, find the best rule with the best covering using dichotomic search. Decreases min_fidelity if needed.",
  ),
  Field(
    "Maximum number of failed attempts to find Fidex rule",
    "max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "30",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used.",
  ),
  Field(
    "Targeted rule fidelity",
    "min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "1.0",
    description: "Rule fidelity accepted when generating a rule.",
  ),
  Field(
    "Lowest rule fidelity accepted",
    "lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    description:
        "Rule fidelity to which we agree to go down during the chosen covering strategy execution.",
  ),
  dropoutDimFld,
  dropoutHypFld,
  nbQuantLevelsFld,
  Field(
    "Normalization file",
    "normalization_file",
    Datatype.filePath,
    description:
        "File containing the mean and std of some attributes. Used to denormalize the rules if specified.",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld,
];

const List<Field> fidexGloRulesFields = [
  rootFolderFld,
  trainDataFileFld,
  trainPredFileFld,
  Field(
    "Train true classes file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train true class file, not mandatory if classes are specified in 'train data file'.",
  ),
  Field(
    "Weights file",
    "weights_file",
    Datatype.filePath,
    description:
        "Weights file (not mandatory if a rules file is given with 'rules file').",
  ),
  Field(
    "Rules file",
    "rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with 'weights file').",
  ),
  rulesOutputFileFld,
  Field(
    "Heuristic",
    "heuristic",
    Datatype.integer,
    description:
        "Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo. (Faster algorithms are less efficient).",
    minValue: "1",
    maxValue: "3",
    defaultValue: "1",
    isRequired: true,
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "File of attributes",
    "attributes_file",
    Datatype.filePath,
    description: "File of attributes.",
  ),
  consoleFileFld,
  Field(
    "Max iteration number",
    "max_iterations",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "10",
    description:
        "Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images.",
  ),
  Field(
    "Minimum covering number",
    "min_covering",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "2",
    description: "Minimum covering number.",
  ),
  Field(
    "Use covering strategy",
    "covering_strategy",
    Datatype.boolean,
    defaultValue: "true",
    description:
        "Whether to use covering strategy : if no rule is found with 'minimum covering', find best rule with best covering using dichotomic search. Decreases min_fidelity if needed.",
  ),
  Field(
    "Maximum failed attempts",
    "max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "30",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used.",
  ),
  Field(
    "Minimal rule fidelity",
    "min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "1.0",
    description: "Minimal rule fidelity accepted when generating a rule.",
  ),
  Field(
    "Lowest minimum fidelity",
    "lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "0.75",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy.",
  ),
  dropoutDimFld,
  dropoutHypFld,
  nbQuantLevelsFld,
  Field(
    "Decision threshold",
    "decision_threshold",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    description:
        "Decision threshold for predictions, you need to specify the index of positive class if you want to use it.",
  ),
  Field(
    "Index of positive class",
    "positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "# classes - 1",
    description:
        "Index of positive class for the usage of decision threshold, index starts at 0.",
  ),
  Field(
    "Normalization file",
    "normalization_file",
    Datatype.filePath,
    description:
        "File containing the mean and std of some attributes. Used to denormalize the rules if specified.",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  Field(
    "Number of threads used for computing the algorithm",
    "nb_threads",
    Datatype.integer,
    minValue: "1",
    maxValue: "# CPU cores",
    description:
        "Number of threads used for computing the algorithm, 1=sequential execution.",
  ),
  seedFld
];

const List<Field> fidexGloStatsFields = [
  rootFolderFld,
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Test data file.",
    isRequired: true,
  ),
  Field(
    "Test prediction file",
    "test_pred_file",
    Datatype.filePath,
    description: "Test prediction file.",
    isRequired: true,
  ),
  Field(
    "Test true classes file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test true classes file, not mandatory if classes are specified in test data file.",
  ),
  Field(
    "Rules input file",
    "global_rules_file",
    Datatype.filePath,
    description: "File containing Rules that will be given as input.",
    isRequired: true,
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "File of attributes",
    "attributes_file",
    Datatype.filePath,
    description:
        "File of attributes. Mandatory if rules file contains attribute names, if not, do not add it.",
  ),
  Field(
    "Statistics file",
    "stats_file",
    Datatype.filePath,
    description: "Output statistic file name.",
  ),
  Field(
    "Output rules file",
    "global_rules_outfile",
    Datatype.filePath,
    description:
        "Global ruleset output file with stats on test set, if you want to compute statistics of global rules on tests set.",
  ),
  consoleFileFld,
  Field(
    "Decision threshold",
    "decision_threshold",
    Datatype.doublePrecision,
    description:
        "Decision threshold for predictions, should be specified if it was previously used in FidexGloRules. You need to specify the index of positive class if you want to use it.",
    minValue: "0",
    maxValue: "1",
  ),
  Field(
    "Index of positive class",
    "positive_class_index",
    Datatype.integer,
    description:
        "Index of positive class for the usage of decision threshold and to compute true/false positive/negative rates, index starts at 0.",
    minValue: "0",
    maxValue: "# classes - 1",
  ),
];

const List<Field> dimlpTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Attributes file",
    "attributes_file",
    Datatype.filePath,
    description: "Path of file containing attributes names.",
  ),
  Field(
    "Validation data file",
    "valid_data_file",
    Datatype.filePath,
    description: "Path of file containing validation data.",
  ),
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Path of file containing the test portion of the dataset.",
  ),
  Field(
    "Pretrained weights file",
    "weights_file",
    Datatype.filePath,
    description: "Path of file containing pretrained weights.",
  ),
  Field(
    "Train true classes file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the train portion of the dataset.",
  ),
  Field(
    "Test true classes file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the test portion of the dataset.",
  ),
  Field(
    "Validation true classes file",
    "valid_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the validation portion of the dataset.",
  ),
  Field(
    "Output weights file",
    "weights_outfile",
    Datatype.filePath,
    defaultValue: "dimlp.wts",
    description: "Path/name of output weights file.",
  ),
  Field(
    "Output train prediction file",
    "train_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpTrain.out",
    description: "Path/name of output train prediction file.",
  ),
  Field(
    "Output test prediction file",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpTest.out",
    description: "Path/name of output test prediction file.",
  ),
  Field(
    "Output validation prediction file",
    "valid_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpValidation.out",
    description: "Path/name of output validation prediction file.",
  ),
  consoleFileFld,
  Field(
    "Output hidden layers file",
    "hidden_layers_outfile",
    Datatype.filePath,
    defaultValue: "hidden_layers.out",
    description: "Path/name of output hidden layers file name.",
  ),
  Field(
    "Number of neurons in the first hidden layer",
    "first_hidden_layer",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description:
        "Number of neurons in the first hidden layer. The default value should be the same as the number of attributes.",
  ),
  Field(
    "Number of neurons in each hidden layer",
    "hidden_layers",
    Datatype.listInteger,
    minValue: "1",
    maxValue: "inf",
    description:
        "Number of neurons in each hidden layer, from the second layer through to the last.",
  ),
  Field(
    "Whether to extract rules with dimlp algorithm",
    "with_rule_extraction",
    Datatype.boolean,
    description: "Whether to extract rules with dimlp algorithm.",
  ),
  rulesOutputFileFld,
  Field(
    "Back-propagation learning rate",
    "learning_rate",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.1",
    description: "Back-propagation learning rate parameter.",
  ),
  Field(
    "Back-propagation momentum parameter",
    "momentum",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.6",
    description: "Back-propagation momentum parameter.",
  ),
  Field(
    "Back-propagation flat spot elimination",
    "flat",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.1",
    description: "Back-propagation flat spot elimination parameter.",
  ),
  nbQuantLevelsFld,
  Field(
    "Error threshold",
    "error_thresh",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    description: "Error threshold to stop training.",
  ),
  Field(
    "Accuracy threshold",
    "acc_thresh",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description: "Accuracy threshold to stop training.",
  ),
  Field(
    "Absolute difference error threshold",
    "abs_error_thresh",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0",
    description:
        "Absolute difference error threshold, 0 if not using this stopping criteria.",
  ),
  Field(
    "Number of epochs",
    "nb_epochs",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "1500",
    description: "Number of epochs.",
  ),
  Field(
    "Number of epochs before showing error",
    "nb_epochs_error",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "10",
    description: "Number of epochs to show error.",
  ),
  Field(
    "Normalization file",
    "normalization_file",
    Datatype.filePath,
    description:
        "Path of file containing the mean and std of some attributes. Used to denormalize the rules if specified.",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld
];

const List<Field> dimlpPredFields = [
  rootFolderFld,
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing the test portion of the dataset.",
  ),
  Field(
    "Weights file",
    "weights_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing weights.",
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Hidden layers file",
    "hidden-layers-file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing hidden layers.",
  ),
  Field(
    "Tests prediction output file",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpTest.out",
    description:
        "Path of file containing the output prediction ran with the test portion of the dataset.",
  ),
  consoleFileFld,
];

const List<Field> dimlpRulFields = [
  rootFolderFld,
  trainDataFileFld,
  Field(
    "Pretrained weights file",
    "weights_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing weights.",
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Hidden layers file",
    "hidden_layers_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of hidden layers file.",
  ),
  Field(
    "Attributes file",
    "attributes_file",
    Datatype.filePath,
    description: "Path of file containing attributes names.",
  ),
  Field(
    "Validation data file",
    "valid_data_file",
    Datatype.filePath,
    description: "Path of file containing validation data.",
  ),
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Path of file containing the test portion of the dataset.",
  ),
  Field(
    "Train true classes file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the train portion of the dataset.",
  ),
  Field(
    "Test true classes file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the test portion of the dataset.",
  ),
  Field(
    "Validation true classes file",
    "valid_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the validation portion of the dataset.",
  ),
  rulesOutputFileFld,
  consoleFileFld,
  Field(
    "Output statistics file",
    "stats_file",
    Datatype.filePath,
    description:
        "Path/name of output file with train, test and validation accuracy.",
  ),
  nbQuantLevelsFld,
  Field(
    "Normalization file",
    "normalization_file",
    Datatype.filePath,
    description:
        "File containing the mean and std of some attributes. Used to denormalize the rules if specified.",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
];

const List<Field> dimlpBTFields = [
  rootFolderFld,
  trainDataFileFld,
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Number of networks",
    "nb_dimlp_nets",
    Datatype.integer,
    defaultValue: "25",
    minValue: "1",
    maxValue: "inf",
    description: "Number of networks.",
  ),
  Field(
    "Attributes file",
    "attributes_file",
    Datatype.filePath,
    description: "Path of file containing attributes names.",
  ),
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Path of test data file.",
  ),
  Field(
    "Train true classes file",
    "train_class_file",
    Datatype.filePath,
    description: "Path of train true class file.",
  ),
  Field(
    "Test true classes file",
    "test_class_file",
    Datatype.filePath,
    description: "Path of test true class file.",
  ),
  consoleFileFld,
  Field(
    "Output weights file",
    "weights_outfilename",
    Datatype.filePath,
    defaultValue: "dimlpBT.wts",
    description: "Path/name of output weights file.",
  ),
  Field(
    "Output train prediction file",
    "train_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpBTTrain.out",
    description: "Path/name of output train prediction file.",
  ),
  Field(
    "Output test prediction file",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpBTTest.out",
    description: "Path/name of output test prediction file.",
  ),
  Field(
    "Output statistics file",
    "stats_file",
    Datatype.filePath,
    description:
        "Output file name with train, test and validation accuracy and with the global train and test accuracy.",
  ),
  Field(
    "Number of neurons in the first hidden layer",
    "first_hidden_layer",
    Datatype.integer,
    minValue: "1",
    maxValue: "# attributes",
    description:
        "Number of neurons in the first hidden layer.",
  ),
  Field(
    "Number of neurons in each hidden layer",
    "hidden_layers",
    Datatype.listInteger,
    minValue: "1",
    maxValue: "inf",
    description:
        "Number of neurons in each hidden layer, from the second layer through to the last.",
  ),
  Field(
    "Output hidden layers file",
    "hidden_layers_outfile",
    Datatype.filePath,
    defaultValue: "hidden_layers.out",
    description: "Output hidden layers file name.",
  ),
  Field(
    "Extract rules with dimlpBT algorithm",
    "with_rule_extraction",
    Datatype.boolean,
    description: "Whether to extract rules with dimlpBT algorithm.",
  ),
  rulesOutputFileFld,
  Field(
    "Back-propagation momentum parameter",
    "momentum",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.6",
    description: "Back-propagation momentum parameter.",
  ),
  Field(
    "Back-propagation flat spot elimination parameter",
    "flat",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.01",
    description: "Back-propagation flat spot elimination parameter.",
  ),
  nbQuantLevelsFld,
  Field(
    "Error threshold",
    "error_thresh",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    description: "Error threshold to stop training.",
  ),
  Field(
    "Accuracy threshold",
    "acc_thresh",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description: "Accuracy threshold to stop training.",
  ),
  Field(
    "Absolute difference error threshold",
    "abs_error_thresh",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    description:
        "Absolute difference error threshold, 0 if not using this stopping criteria.",
  ),
  Field(
    "Number of epochs",
    "nb_epochs",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description: "Number of epochs.",
  ),
  Field(
    "Number of epochs before showing error",
    "nb_epochs_error",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description: "Number of epochs to show error.",
  ),
  Field(
    "Number of examples for one single network",
    "nb_ex_per_net",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description:
        "Number of examples for one single network, 0 for all examples, it is not recommended to change this value.",
  ),
  Field(
    "Normalization file",
    "normalization_file",
    Datatype.filePath,
    description:
        "Path of file containing the mean and std of some attributes. Used to denormalize the rules if specified.",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld,
];

const List<Field> dimlpClsFields = [
  rootFolderFld,
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing the test portion of the dataset.",
  ),
  Field(
    "Weights file",
    "weights_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing weights.",
  ),
  Field(
    "Number of attributes",
    "nb_attributes",
    Datatype.integer,
    isRequired: true,
    minValue: "1",
    maxValue: "inf",
    description: "Number of input neurons.",
  ),
  Field(
    "Number of classes",
    "nb_classes",
    Datatype.integer,
    isRequired: true,
    minValue: "2",
    maxValue: "inf",
    description: "Number of output neurons.",
  ),
  Field(
    "Hidden layers file",
    "hidden_layers_file",
    Datatype.filePath,
    isRequired: true,
    description: "Hidden layers file name.",
  ),
  Field(
    "Test true class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the test portion of the dataset.",
  ),
  Field(
    "Tests prediction output file",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpTest.out",
    description:
        "Path of output file containing predictions for the tests portion of the dataset.",
  ),
  consoleFileFld,
  Field(
    "Stats file",
    "stats_file",
    Datatype.filePath,
    description: "Path of output file name containing statistics.",
  ),
  Field(
    "First hidden layer values file",
    "hid_file",
    Datatype.filePath,
    defaultValue: "dimlpTest.hid",
    description: "Path of output file with first hidden layer values.",
  ),
  nbQuantLevelsFld,
];

const List<Field> computeRocCurveFields = [
  rootFolderFld,
  Field(
    "Test class file",
    "test_class_file",
    Datatype.filePath,
    description: "Test class file",
    isRequired: true,
  ),
  Field(
    "Test prediction file",
    "test_pred_file",
    Datatype.filePath,
    description: "Test prediction file",
    isRequired: true,
  ),
  nbClassesFld,
  Field(
    "Index of positive class",
    "positive_class_index",
    Datatype.integer,
    minValue: "1",
    maxValue: "nb_classes-1",
    description: "Index of positive class, index starts at 0",
    isRequired: true,
  ),
  Field(
    "Output statistic file name with AUC score",
    "stats_file",
    Datatype.filePath,
    description: "Output statistic file name with AUC score",
  ),
  Field(
    "Whether to show parameters",
    "show_params",
    Datatype.boolean,
    defaultValue: "true",
    description: "Whether to show parameters",
  ),
  Field(
    "Name of estimator",
    "estimator",
    Datatype.string,
    description: "Name of estimator",
  ),
  Field(
    "Output ROC curve file name",
    "output_roc",
    Datatype.filePath,
    defaultValue: "roc_curve.png",
    description: "Output ROC curve file name",
  ),
];

const List<Field> convKerasFields = [
  rootFolderFld,
  trainDataFileFld,
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Test data file",
    isRequired: true,
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Dataset",
    "dataset",
    Datatype.string,
    description: "Dataset",
  ),
  Field(
    "Train class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train class file, mandatory if classes are not specified in train_data_file",
  ),
  Field(
    "Test class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test class file, mandatory if classes are not specified in test_data_file",
  ),
  Field(
    "Percentage of train data taken for validation",
    "valid_ratio",
    Datatype.doublePrecision,
    minValue: "0",
    description: "Percentage of train data taken for validation",
  ),
  Field(
    "Validation data file",
    "valid_data_file",
    Datatype.filePath,
    description: "Validation data file",
  ),
  Field(
    "Validation class file",
    "valid_class_file",
    Datatype.filePath,
    description:
        "Validation class file, mandatory if classes are not specified in valid_data_file. BE CAREFUL if there is validation files, and you want to use Fidex algorithms later, you will have to use both train and validation datas given here in the train datas and classes of Fidex",
  ),
  Field(
    "Whether input image datas are normalized between 0 and 1",
    "normalized",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether input image datas are normalized between 0 and 1",
  ),
  Field(
    "Number of training epochs",
    "nb_epochs",
    Datatype.integer,
    minValue: "1",
    defaultValue: "80",
    description: "Number of training epochs",
  ),
  Field(
    "Output train and validation (in this order) prediction file name",
    "train_valid_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTrain.out",
    description:
        "Output train and validation (in this order) prediction file name",
  ),
  Field(
    "Output test prediction file name",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTest.out",
    description: "Output test prediction file name",
  ),
  Field(
    "Output weights file name",
    "weights_outfile",
    Datatype.filePath,
    defaultValue: "weights.wts",
    description: "Output weights file name",
  ),
  Field(
    "Output statistic file name with train and test accuracy",
    "stats_file",
    Datatype.filePath,
    defaultValue: "stats.txt",
    description:
        "Output statistic file name with train and test accuracy",
  ),
  consoleFileFld,
  nbQuantLevelsFld,
  Field(
    "Parameter to improve dynamics",
    "K",
    Datatype.doublePrecision,
    minValue: "0",
    defaultValue: "1.0",
    description: "Parameter to improve dynamics",
  ),
  Field(
    "Whether to change 3-channels data from RGB to HSL format",
    "with_hsl",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to change 3-channels data from RGB to HSL format",
  ),
  Field(
    "Whether to train with ResNet",
    "with_resnet",
    Datatype.boolean,
    defaultValue: "false",
    description: "Whether to train with ResNet",
  ),
  Field(
    "Whether to train with VGG",
    "with_vgg",
    Datatype.boolean,
    defaultValue: "false",
    description: "Whether to train with VGG",
  ),
];

const List<Field> gradBoostTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Test data file",
    isRequired: true,
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Train class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train class file, mandatory if classes are not specified in train_data_file",
  ),
  Field(
    "Output train prediction file name",
    "train_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTrain.out",
    description: "Output train prediction file name.",
  ),
  Field(
    "Test class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test class file, mandatory if classes are not specified in test_data_file",
  ),
  Field(
    "Output test prediction file name",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTest.out",
    description: "Output test prediction file name.",
  ),
  consoleFileFld,
  Field(
    "Output statistic file name with train and test accuracy",
    "stats_file",
    Datatype.filePath,
    defaultValue: "stats.txt",
    description: "Output statistic file name with train and test accuracy.",
  ),
  rulesOutputFileFld,
  Field(
    "Number of generated trees in the forest",
    "n_estimators",
    Datatype.integer,
    minValue: "1",
    defaultValue: "100",
    description: "Number of generated trees in the forest.",
  ),
  Field(
    "Loss function to be optimized",
    "loss",
    Datatype.string,
    defaultValue: "log_loss",
    description:
        "Loss function to be optimized. Choices are 'log_loss' or 'exponential'.",
  ),
  Field(
    "Shrinks the contribution of each tree",
    "learning_rate",
    Datatype.doublePrecision,
    minValue: "0",
    defaultValue: "0.1",
    description: "Shrinks the contribution of each tree",
  ),
  Field(
    "Fraction of samples to be used for fitting the individual base learners",
    "subsample",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "1.0",
    description:
        "Fraction of samples to be used for fitting the individual base learners",
  ),
  Field(
    "Function to measure split quality",
    "criterion",
    Datatype.string,
    defaultValue: "friedman_mse",
    description:
        "Function to measure split quality. Choices are 'friedman_mse' or 'squared_error'.",
  ),
  Field(
    "Maximum depth of the individual regression estimators",
    "max_depth",
    Datatype.string,
    defaultValue: "3",
    description: "Maximum depth of the individual regression estimators.",
  ),
  Field(
    "Minimum number of samples required to split an internal node",
    "min_samples_split",
    Datatype.string,
    defaultValue: "2",
    description:
        "Minimum number of samples required to split an internal node, if float, it is a fraction of the number of samples.",
  ),
  Field(
    "Minimum number of samples required to be at a leaf node",
    "min_samples_leaf",
    Datatype.string,
    defaultValue: "1",
    description:
        "Minimum number of samples required to be at a leaf node, if float, it is a fraction of the number of samples.",
  ),
  Field(
    "Minimum weighted fraction of the sum total of input samples weights required to be at a leaf node",
    "min_weight_fraction_leaf",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "0.5",
    defaultValue: "0.0",
    description:
        "Minimum weighted fraction of the sum total of input samples weights required to be at a leaf node.",
  ),
  Field(
    "Number of features to consider when looking for the best split",
    "max_features",
    Datatype.string,
    defaultValue: "sqrt",
    description:
        "Number of features to consider when looking for the best split if float, it is a fraction of the number of features. 1 stands for 1 feature, for all features put 'all', not 1.0.",
  ),
  Field(
    "Grow trees with max_leaf_nodes in best-first fashion",
    "max_leaf_nodes",
    Datatype.integer,
    minValue: "2",
    maxValue: "inf",
    description: "Grow trees with max_leaf_nodes in best-first fashion",
  ),
  Field(
    "A node will be split if this split induces a decrease of the impurity greater than or equal to this value",
    "min_impurity_decrease",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "inf",
    defaultValue: "0.0",
    description:
        "A node will be split if this split induces a decrease of the impurity greater than or equal to this value.",
  ),
  Field(
    "Estimator object used to compute the initial predictions",
    "init",
    Datatype.string,
    defaultValue: "zero",
    description: "Estimator object used to compute the initial predictions",
  ),
  seedFld,
  Field(
    "Controls the verbosity when fitting and predicting",
    "verbose",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0",
    description: "Controls the verbosity when fitting and predicting.",
  ),
  Field(
    "Whether to reuse the solution of the previous call to fit and add more estimators to the ensemble",
    "warm_start",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to reuse the solution of the previous call to fit and add more estimators to the ensemble.",
  ),
  Field(
    "Proportion of training data to set aside as validation set for early stopping",
    "validation_fraction",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "0.1",
    description:
        "Proportion of training data to set aside as validation set for early stopping.",
  ),
  Field(
    "Decide if early stopping will be used to terminate training when validation score is not improving",
    "n_iter_no_change",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description:
        "Decide if early stopping will be used to terminate training when validation score is not improving, stopping if validation doesn't improve during this number of iterations",
  ),
  Field(
    "Tolerance for the early stopping",
    "tol",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.0001",
    description: "Tolerance for the early stopping.",
  ),
  Field(
    "Complexity parameter used for Minimal Cost-Complexity Pruning",
    "ccp_alpha",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.0",
    description:
        "Complexity parameter used for Minimal Cost-Complexity Pruning.",
  ),
];

const List<Field> mlpTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Train class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train class file, mendatory if classes are not specified in train_data_file",
  ),
  Field(
    "Output train prediction file name",
    "train_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTrain.out",
    description: "Output train prediction file name",
  ),
  Field(
    "Test class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test class file, mendatory if classes are not specified in test_data_file",
  ),
  Field(
    "Output test prediction file name",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTest.out",
    description: "Output test prediction file name",
  ),
  consoleFileFld,
  Field(
    "Output statistics file",
    "stats_file",
    Datatype.filePath,
    defaultValue: "stats.txt",
    description:
        "Output file name with train and test accuracy",
  ),
  Field(
    "Output weights file name",
    "weights_outfile",
    Datatype.filePath,
    defaultValue: "weights.wts",
    description: "Output weights file name",
  ),
  nbQuantLevelsFld,
  Field(
    "Parameter to improve dynamics",
    "K",
    Datatype.doublePrecision,
    defaultValue: "1.0",
    description: "Parameter to improve dynamics",
  ),
  Field(
    "Size of each hidden layers",
    "hidden_layer_sizes",
    Datatype.listInteger,
    defaultValue: "100",
    description: "Size of each hidden layers",
  ),
  Field(
    "Activation function",
    "activation",
    Datatype.string, // TODO
    defaultValue: "relu",
    description: "Activation function",
  ),
  Field(
    "Solver for weight optimization",
    "solver",
    Datatype.string, // TODO
    defaultValue: "adam",
    description: "Solver for weight optimization",
  ),
  Field(
    "Strength of the L2 regularization term",
    "alpha",
    Datatype.doublePrecision,
    defaultValue: "0.0001",
    description: "Strength of the L2 regularization term",
  ),
  Field(
    "Size of minibatches for stochastic optimizers for adam and sgd",
    "batch_size",
    Datatype.integer, //TODO
    defaultValue: "auto",
    description:
        "Size of minibatches for stochastic optimizers for adam and sgd",
  ),
  Field(
    "Learning rate schedule for weight updates for sgd solver",
    "learning_rate",
    Datatype.string, // TODO
    defaultValue: "constant",
    description:
        "Learning rate schedule for weight updates for sgd solver",
  ),
  Field(
    "Initial learning rate for adam and sgd",
    "learning_rate_init",
    Datatype.doublePrecision,
    defaultValue: "0.001",
    description: "Initial learning rate for adam and sgd",
  ),
  Field(
    "Exponent for inverse scaling learning rate for sgd",
    "power_t",
    Datatype.doublePrecision,
    defaultValue: "0.5",
    description:
        "Exponent for inverse scaling learning rate for sgd",
  ),
  Field(
    "Maximum number of iterations",
    "max_iterations",
    Datatype.integer,
    defaultValue: "200",
    description: "Maximum number of iterations",
  ),
  Field(
    "Whether to shuffle samples in each iteration for sgd and adam",
    "shuffle",
    Datatype.boolean,
    defaultValue: "true",
    description:
        "Whether to shuffle samples in each iteration for sgd and adam",
  ),
  seedFld,
  Field(
    "Tolerance for optimization",
    "tol",
    Datatype.doublePrecision,
    defaultValue: "0.0001",
    description: "Tolerance for optimization",
  ),
  Field(
    "Enable verbose output",
    "verbose",
    Datatype.boolean,
    defaultValue: "false",
    description: "Enable verbose output",
  ),
  Field(
    "Whether to reuse previous solution to fit initialization",
    "warm_start",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to reuse previous solution to fit initialization",
  ),
  Field(
    "Momentum for gradient descent update for sgd",
    "momentum",
    Datatype.doublePrecision,
    defaultValue: "0.9",
    description: "Momentum for gradient descent update for sgd",
  ),
  Field(
    "Whether to use Nesterov’s momentum for sgd and momentum",
    "nesterovs_momentum",
    Datatype.boolean,
    defaultValue: "true",
    description:
        "Whether to use Nesterov’s momentum for sgd and momentum",
  ),
  Field(
    "Whether to use early stopping to terminate training when validation score is not improving for sgd and adam",
    "early_stopping",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to use early stopping to terminate training when validation score is not improving for sgd and adam",
  ),
  Field(
    "Proportion of training data to set aside as validation set for early stopping",
    "validation_fraction",
    Datatype.doublePrecision,
    defaultValue: "0.1",
    description:
        "Proportion of training data to set aside as validation set for early stopping",
  ),
  Field(
    "Exponential decay rate for estimates of first moment vector in adam",
    "beta_1",
    Datatype.doublePrecision,
    defaultValue: "0.9",
    description:
        "Exponential decay rate for estimates of first moment vector in adam",
  ),
  Field(
    "Exponential decay rate for estimates of second moment vector in adam",
    "beta_2",
    Datatype.doublePrecision,
    defaultValue: "0.999",
    description:
        "Exponential decay rate for estimates of second moment vector in adam",
  ),
  Field(
    "Value for numerical stability in adam",
    "epsilon",
    Datatype.doublePrecision,
    defaultValue: "1e-08",
    description: "Value for numerical stability in adam",
  ),
  Field(
    "Maximum number of epochs to not meet tol improvement for sgd and adam",
    "n_iter_no_change",
    Datatype.integer,
    defaultValue: "10",
    description:
        "Maximum number of epochs to not meet tol improvement for sgd and adam",
  ),
  Field(
    "Maximum number of loss function calls for lbfgs",
    "max_fun",
    Datatype.integer,
    defaultValue: "15000",
    description:
        "Maximum number of loss function calls for lbfgs",
  ),
];

const List<Field> randForestsTrnFields = [
  rootFolderFld,
  Field(
    "Train data file",
    "train_data_file",
    Datatype.filePath,
    description: "Train data file",
    isRequired: true,
  ),
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Test data file",
    isRequired: true,
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Train class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train class file, mandatory if classes are not specified in train_data_file",
  ),
  Field(
    "Output train prediction file",
    "train_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTrain.out",
    description: "Output train prediction file name",
  ),
  Field(
    "Output test prediction file",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTest.out",
    description: "Output test prediction file name",
  ),
  consoleFileFld,
  Field(
    "Output statistics file",
    "stats_file",
    Datatype.filePath,
    defaultValue: "stats.txt",
    description:
        "Output file name with train and test accuracy.",
  ),
  rulesOutputFileFld,
  Field(
    "Number of generated trees in the forest",
    "n_estimators",
    Datatype.integer,
    defaultValue: "100",
    minValue: "1",
    maxValue: "inf",
    description: "Number of generated trees in the forest.",
  ),
  Field(
    "Function to measure split quality",
    "criterion",
    Datatype.restrictedChoiceString,
    items: ["gini", "entropy", "log_loss"],
    defaultValue: "gini",
    description: "Function to measure split quality.",
  ),
  Field(
    "Maximum depth of the tree",
    "max_depth",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description: "Maximum depth of the tree",
  ),
  //TODO Handle differenciation between float and int
  Field(
    "Minimum number of samples to split an internal node",
    "min_samples_split",
    Datatype.doublePrecision,
    defaultValue: "2",
    minValue: "2",
    maxValue: "inf",
    description:
        "Minimum number of samples required to split an internal node, if float, it is a fraction of the number of samples.",
  ),
  //TODO Handle differenciation between float and int
  Field(
    "Minimum number of samples to be at a leaf",
    "min_samples_leaf",
    Datatype.doublePrecision,
    defaultValue: "1",
    minValue: "1",
    maxValue: "inf",
    description:
        "Minimum number of samples required to be at a leaf node, if float, it is a fraction of the number of samples",
  ),
  Field(
    "Minimum weighted fraction",
    "min_weight_fraction_leaf",
    Datatype.doublePrecision,
    defaultValue: "0.0",
    minValue: "0.0",
    maxValue: "0.5",
    description:
        "Minimum weighted fraction of the sum total of input samples weights required to be at a leaf node",
  ),
  // TODO: handle this input to fit float & int options
  Field(
    "Number of features to consider when looking for the best split",
    "max_features",
    Datatype.restrictedChoiceString,
    defaultValue: "sqrt",
    items: ["sqrt", "log2", "all"],
    description:
        "(ONLY CATEGORIES ARE AVAILABLE YET, NO FLOAT OF INT OPTIONS. DO IT MANUALLY IF NECESSARY)\nNumber of features to consider when looking for the best split. If float, it is a fraction of the number of features. 1 stands for 1 feature, for all features put 'all', not 1.0",
  ),
  Field(
    "Maximum number of leaf nodes",
    "max_leaf_nodes",
    minValue: "2",
    maxValue: "inf",
    Datatype.integer,
    description: "Grow trees with max_leaf_nodes in best-first fashion",
  ),
  Field(
    "Minimal impurity decrease",
    "min_impurity_decrease",
    Datatype.doublePrecision,
    defaultValue: "0.0",
    minValue: "0.0",
    maxValue: "inf",
    description:
        "A node will be split if this split induces a decrease of the impurity greater than or equal to this value.",
  ),
  Field(
    "Use bootstrap samples",
    "bootstrap",
    Datatype.boolean,
    defaultValue: "true",
    description:
        "Whether bootstrap samples are used when building trees",
  ),
  Field(
    "Whether to use out-of-bag samples",
    "oob_score",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to use out-of-bag samples to estimate the generalization score",
  ),
  Field(
    "Number of parallel jobs",
    "n_jobs",
    Datatype.integer,
    defaultValue: "1",
    minValue: "-1",
    maxValue: "# CPUs available",
    description:
        "Number of jobs to run in parallel, -1 = using all processors",
  ),
  seedFld,
  Field(
    "Verbose level",
    "verbose",
    Datatype.integer,
    defaultValue: "0",
    minValue: "0",
    maxValue: "inf",
    description: "Controls the verbosity when fitting and predicting",
  ),
  Field(
    "Reuse the solution of the previous calls",
    "warm_start",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to reuse the solution of the previous call to fit and add more estimators to the ensemble.",
  ),
  Field(
    "Class balance",
    "class_weight",
    Datatype.string,
    description:
        "Class balance, for example with a dictionary and 2 classes: {0:1.2, 1:3.5}",
  ),
  Field(
    "Complexity parameter",
    "ccp_alpha",
    Datatype.doublePrecision,
    defaultValue: "0.0",
    minValue: "0.0",
    maxValue: "inf",
    description:
        "Complexity parameter used for Minimal Cost-Complexity Pruning.",
  ),
  Field(
    "Number number of samples",
    "max_samples",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    description:
        "Number of samples to draw to train each base estimator for bootstrap, if float, it is a fraction of the number of samples",
  ),
];

const List<Field> svmTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  Field(
    "Test data file",
    "test_data_file",
    Datatype.filePath,
    description: "Test data file",
    isRequired: true,
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Train class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train class file, mandatory if classes are not specified in train_data_file",
  ),
  Field(
    "Output train prediction file",
    "train_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTrain.out",
    description: "Output train prediction file name",
  ),
  Field(
    "Test class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test class file, mandatory if classes are not specified in test_data_file",
  ),
  consoleFileFld,
  Field(
    "Output statistics file",
    "stats_file",
    Datatype.filePath,
    defaultValue: "stats.txt",
    description:
        "Output file name with train and test accuracy",
  ),
  Field(
    "Output weights file name",
    "weights_outfile",
    Datatype.filePath,
    defaultValue: "weights.wts",
    description: "Output weights file name",
  ),
  Field(
    "Output ROC curve file name",
    "output_roc",
    Datatype.filePath,
    defaultValue: "roc_curve.png",
    description: "Output ROC curve file name",
  ),
  nbQuantLevelsFld,
  Field(
    "Parameter to improve dynamics",
    "K",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "1.0",
    description: "Parameter to improve dynamics",
  ),
  Field(
    "Index of positive class",
    "positive_class_index",
    minValue: "0",
    maxValue: "# classes -1",
    Datatype.integer,
    description:
        "Index of positive class for the roc curve calculation, index starts at 0",
  ),
  Field(
    "Whether to return ROC statistics",
    "return_roc",
    Datatype.boolean,
    defaultValue: "false",
    description: "Whether to return ROC statistics",
  ),
  Field(
    "Regularization",
    "C",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "inf",
    defaultValue: "1.0",
    description: "Regularization.",
  ),
  Field(
    "Kernel type",
    "kernel",
    Datatype.restrictedChoiceString,
    defaultValue: "rbf",
    items: ["linear", "poly", "rbf", "sigmoid"],
    description: "Kernel type.",
  ),
  Field(
    "Polynomial degree",
    "degree",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "3",
    description: "Polynomial degree.",
  ),
  Field(
    "Gamma predefined value (do not use with the custom field below)",
    "gamma_1",
    Datatype.restrictedChoiceString,
    defaultValue: "scale",
    items: ["", "scale", "auto"],
    description: "Gamma predefined value.",
  ),
  Field(
    "Gamma custom value (do not use with the predefined field above)",
    "gamma_2",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "inf",
    description: "Gamma custom floating point value.",
  ),
  Field(
    "Term in kernel function",
    "coef0",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "inf",
    defaultValue: "0.0",
    description: "Term in kernel function.",
  ),
  Field(
    "Use shrinking heuristic",
    "shrinking",
    Datatype.boolean,
    defaultValue: "true",
    description: "Whether to use shrinking heuristic.",
  ),
  Field(
    "Tolerance for stopping criterion",
    "tol",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "inf",
    defaultValue: "0.001",
    description: "Tolerance for stopping criterion.",
  ),
  Field(
    "Kernel cache size(MB)",
    "cache_size",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "inf",
    defaultValue: "200",
    description: "Kernel cache size(MB).",
  ),
  Field(
    "Class balance",
    "class_weight",
    Datatype.dictionary,
    description:
        "Class balance, for example with a dictionary and 2 classes: {0:1.2, 1:3.5}",
  ),
  Field(
    "Enable verbose output",
    "verbose",
    Datatype.boolean,
    defaultValue: "false",
    description: "Whether to enable verbose output.",
  ),
  Field(
    "Maximal number of iterations",
    "max_iterations",
    Datatype.integer,
    minValue: "-1",
    maxValue: "inf",
    defaultValue: "-1",
    description: "Maximal number of iterations, -1 designates no limit.",
  ),
  Field(
    "Decision function shape",
    "decision_function_shape",
    Datatype.restrictedChoiceString,
    defaultValue: "ovr",
    items: ["ovr (one-vs-rest)", "ovo (one-vs-one)"],
    description: "Decision function shape.",
  ),
  Field(
    "Whether to break tie decision",
    "break_ties",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to break tie decision for ovr with more than 2 classes.",
  ),
];

const List<Field> normalizationFields = [
  rootFolderFld,
  nbAttributesFld,
  nbClassesFld,
  Field(
    "File of attributes",
    "attributes_file",
    Datatype.filePath,
    description:
        "File of attributes, mandatory if rules or normalization stats are written with attribute names",
  ),
  Field(
    "String representing a missing value in your data",
    "missing_values",
    Datatype.string,
    description:
        "String representing a missing value in your data, put 'NaN' (or any string not present in your data) if you do not have any missing value, mandatory for normalization",
  ),
  Field(
    "Normalization file",
    "normalization_file",
    Datatype.filePath,
    description:
        "File containing the mean and standard deviation of some attributes, used for normalization and denormalization if specified",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  Field(
    "Data files to normalize",
    "data_files",
    Datatype.listFilePath,
    description:
        "Data files to normalize, they are normalized with respect to the first one if normalization_file is not specified",
  ),
  Field(
    "Rule files to denormalize",
    "rule_files",
    Datatype.listFilePath,
    description:
        "Rule files to denormalize, denormalization is possible only if a normalization_file file or mus, sigmas and normalization_indices are given. Either 'data_files' or 'rule_files' must be specified",
  ),
  Field(
    "Output normalization file",
    "output_normalization_file",
    Datatype.filePath,
    defaultValue: "normalization_stats.txt",
    description:
        "Output file name containing the mean and std of the normalized attributes when normalization_file is not specified",
  ),
  Field(
    "Normalized files names",
    "output_data_files",
    Datatype.listFilePath,
    description:
        "Normalized files names, it is mandatory to specify everyone of them if one is specified (default: <original_name>_normalized<original_extension>)",
  ),
  Field(
    "Normalized rule files names",
    "output_rule_files",
    Datatype.listFilePath,
    description:
        "Normalized rule files names, it is mandatory to specify everyone of them if one is specified (default: <original_name>_denormalized<original_extension>)",
  ),
  Field(
    "Whether we use median instead of mean to compute normalization",
    "with_median",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether we use median instead of mean to compute normalitzation",
  ),
  Field(
    "Whether to fill missing values with mean or median during normalization",
    "fill_missing_values",
    Datatype.boolean,
    defaultValue: "true",
    description:
        "Whether we fill missing values with mean or median during normalization",
  ),
];
