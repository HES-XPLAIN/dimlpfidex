import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/data/common_fields_data.dart';

// TODO: complete datas (must have)

const List<Field> fidexFields = [
  rootFolderFld,
  trainDataFileFld,
  trainPredFileFld,
  Field(
    "Train true class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train true class file, not mandatory if classes are specified in train data file",
  ),
  Field(
    "Test sample(s) data file",
    "test_data_file",
    Datatype.filePath,
    isRequired: true,
    description:
        "Test sample(s) data file with data, prediction (if no --test_pred_file) and true class (if no --test_class_file)",
  ),
  Field(
    "Weights file",
    "weights_file",
    Datatype.filePath,
    description:
        "Weights file (not mandatory if a rules file is given with --rules_file)",
  ),
  Field(
    "Rules file",
    "rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)",
  ),
  rulesOutputFileFld,
  nbAttributesFld,
  nbClassesFld,
  Field(
    "Test prediction file",
    "test_pred_file",
    Datatype.filePath,
    description: "Test prediction file",
  ),
  Field(
    "Test true class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test true class file. If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case)",
  ),
  Field(
    "File of attributes",
    "attributes_file",
    Datatype.filePath,
    description: "File of attributes",
  ),
  Field(
    "Output statistic file name",
    "stats_file",
    Datatype.filePath,
    description: "Output statistic file name",
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
    "Whether to use this strategy",
    "covering_strategy",
    Datatype.boolean,
    description:
        "Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)",
  ),
  Field(
    "Maximum number of failed attempts",
    "max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "30",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)",
  ),
  Field(
    "Minimal rule fidelity accepted",
    "min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "1.0",
    description:
        "Minimal rule fidelity accepted when generating a rule (default: 1.0)",
  ),
  Field(
    "Minimal min_fidelity",
    "lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.75",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)",
  ),
  Field(
    "Dimension dropout parameter",
    "dropout_dim",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.0",
    description: "Dimension dropout parameter (default: 0.0)",
  ),
  Field(
    "Hyperplan dropout parameter",
    "dropout_hyp",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.0",
    description: "Hyperplan dropout parameter (default: 0.0)",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    maxValue: "inf",
    defaultValue: "50",
    description:
        "Number of stairs in staircase activation function (default: 50)",
  ),
  Field(
    "Decision threshold for predictions",
    "decision_threshold",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Decision threshold for predictions, you need to specify the index of positive class if you want to use it",
  ),
  Field(
    "Index of positive class",
    "positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "nb_classes - 1",
    description:
        "Index of positive class for the usage of decision threshold, index starts at 0",
  ),
  Field(
    "File containing the mean and std",
    "normalization_file",
    Datatype.filePath,
    description:
        "File containing the mean and std of some attributes. Used to denormalize the rules if specified",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld,
];

const List<Field> fidexGloFields = [
  rootFolderFld,
  Field(
    "Test sample(s) data file with data and prediction",
    "test_data_file",
    Datatype.filePath,
    description:
        "Test sample(s) data file with data and prediction. If no --test_pred_file is specified, classes may be included here if launching with fidex(--with_fidex).",
  ),
  Field(
    "Ruleset input file",
    "global_rules_file",
    Datatype.filePath,
    description: "Ruleset input file.",
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
    "Test prediction file",
    "test_pred_file",
    Datatype.filePath,
    description:
        "Test prediction file. If given, --test_data_file needs to have only test data.",
  ),
  Field(
    "Explanation(s) output file name",
    "explanation_file",
    Datatype.filePath,
    description: "Explanation(s) output file name.",
  ),
  consoleFileFld,
  Field(
    "Decision threshold for predictions",
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
        "Weights file. Not mandatory if a rules file is given with --rules_file.",
  ),
  Field(
    "Rules file",
    "rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus. Not mandatory if a weights file is given with --weights_file.",
  ),
  Field(
    "Test true class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test true class file. Classes can be specified in the test data file.",
  ),
  Field(
    "Max iteration number",
    "max_iterations",
    Datatype.integer,
    minValue: "1",
    defaultValue: "10",
    description:
        "Max iteration number, also the max possible number of attributes in a rule. Should be 25 if working with images (default: 10).",
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
    defaultValue: "false",
    description:
        "Whether to use this strategy: if no rule is found with min_covering, find the best rule with the best covering using dichotomic search. Decreases min_fidelity if needed (default: True).",
  ),
  Field(
    "Maximum number of failed attempts to find Fidex rule",
    "max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30).",
  ),
  Field(
    "Minimal rule fidelity accepted when generating a rule",
    "min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Minimal rule fidelity accepted when generating a rule (default: 1.0).",
  ),
  Field(
    "Minimal min_fidelity to which we agree to go down during covering_strategy",
    "lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75).",
  ),
  Field(
    "Dimension dropout parameter",
    "dropout_dim",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description: "Dimension dropout parameter (default: 0.0).",
  ),
  Field(
    "Hyperplan dropout parameter",
    "dropout_hyp",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description: "Hyperplan dropout parameter (default: 0.0).",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    description:
        "Number of stairs in staircase activation function (default: 50).",
  ),
  Field(
    "File containing the mean and std of some attributes",
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
    "Train true class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Train true class file, not mandatory if classes are specified in train data file.",
  ),
  Field(
    "Weights file",
    "weights_file",
    Datatype.filePath,
    description:
        "Weights file (not mandatory if a rules file is given with --rules_file).",
  ),
  Field(
    "Rules file to be converted to hyperlocus",
    "rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file).",
  ),
  Field(
    "Rules output file",
    "global_rules_outfile",
    Datatype.filePath,
    description: "Rules output file.",
    isRequired: true,
  ),
  Field(
    "Heuristic",
    "heuristic",
    Datatype.integer,
    description:
        "Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo.",
    minValue: "1",
    maxValue: "3",
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
    "Whether to use this strategy",
    "covering_strategy",
    Datatype.boolean,
    description:
        "Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True).",
  ),
  Field(
    "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used",
    "max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "30",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used.",
  ),
  Field(
    "Minimal rule fidelity accepted when generating a rule",
    "min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "1.0",
    description: "Minimal rule fidelity accepted when generating a rule.",
  ),
  Field(
    "Minimal min_fidelity to which we agree to go down during covering_strategy",
    "lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.75",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy.",
  ),
  Field(
    "Dimension dropout parameter",
    "dropout_dim",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "0.0",
    description: "Dimension dropout parameter.",
  ),
  Field(
    "Hyperplan dropout parameter",
    "dropout_hyp",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "0.0",
    description: "Hyperplan dropout parameter.",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    maxValue: "inf",
    defaultValue: "50",
    description: "Number of stairs in staircase activation function.",
  ),
  Field(
    "Decision threshold for predictions",
    "decision_threshold",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Decision threshold for predictions, you need to specify the index of positive class if you want to use it.",
  ),
  Field(
    "Index of positive class for the usage of decision threshold",
    "positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "# classes - 1",
    description:
        "Index of positive class for the usage of decision threshold, index starts at 0.",
  ),
  Field(
    "File containing the mean and std of some attributes",
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
    "Test true class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Test true class file, not mandatory if classes are specified in test data file.",
  ),
  Field(
    "Ruleset input file",
    "global_rules_file",
    Datatype.filePath,
    description: "Ruleset input file.",
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
    "Output statistic file name",
    "stats_file",
    Datatype.filePath,
    description: "Output statistic file name.",
  ),
  Field(
    "Global ruleset output file with stats on test set",
    "global_rules_outfile",
    Datatype.filePath,
    description:
        "Global ruleset output file with stats on test set, if you want to compute statistics of global rules on tests set.",
  ),
  consoleFileFld,
  Field(
    "Decision threshold for predictions",
    "decision_threshold",
    Datatype.doublePrecision,
    description:
        "Decision threshold for predictions, use if it was used in FidexGloRules, you need to specify the index of positive class if you want to use it.",
    minValue: "0",
    maxValue: "1",
  ),
  Field(
    "Index of positive class for the usage of decision threshold and to compute true/false positive/negative rates",
    "positive_class_index",
    Datatype.integer,
    description:
        "Index of positive class for the usage of decision threshold and to compute true/false positive/negative rates, index starts at 0.",
    minValue: "0",
    maxValue: "nb_classes-1",
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
    "Train true class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the train portion of the dataset.",
  ),
  Field(
    "Test true class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the test portion of the dataset.",
  ),
  Field(
    "Validation true class file",
    "valid_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the validation portion of the dataset.",
  ),
  Field(
    "Output weights file name",
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
  Field(
    "Rules output file",
    "global_rules_outfile",
    Datatype.filePath,
    description: "Path/name of rules output file.",
  ),
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
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    defaultValue: "50",
    maxValue: "inf",
    description: "Number of stairs in staircase activation function.",
  ),
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
    "Number of epochs to show error",
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
    "Train true class file",
    "train_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the train portion of the dataset.",
  ),
  Field(
    "Test true class file",
    "test_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the test portion of the dataset.",
  ),
  Field(
    "Validation true class file",
    "valid_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the validation portion of the dataset.",
  ),
  Field(
    "Rules output file",
    "global_rules_outfile",
    Datatype.filePath,
    defaultValue: "dimlp.rls",
    description: "Path/name of rules output file.",
  ),
  consoleFileFld,
  Field(
    "Stats file",
    "stats_file",
    Datatype.filePath,
    description:
        "Path/name of output file with train, test and validation accuracy.",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    defaultValue: "50",
    maxValue: "inf",
    description: "Number of stairs in staircase activation function.",
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
    "Train true class file",
    "train_class_file",
    Datatype.filePath,
    description: "Path of train true class file.",
  ),
  Field(
    "Test true class file",
    "test_class_file",
    Datatype.filePath,
    description: "Path of test true class file.",
  ),
  consoleFileFld,
  Field(
    "Output weights file name",
    "weights_outfilename",
    Datatype.filePath,
    defaultValue: "dimlpBT.wts",
    description: "Path/name of output weights file.",
  ),
  Field(
    "Output train prediction file name",
    "train_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpBTTrain.out",
    description: "Path/name of output train prediction file.",
  ),
  Field(
    "Output test prediction file name",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpBTTest.out",
    description: "Path/name of output test prediction file.",
  ),
  Field(
    "Output file name with train, test and validation accuracy and with the global train and test accuracy",
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
        "Number of neurons in the first hidden layer (default: nb_attributes).",
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
    "Output hidden layers file name",
    "hidden_layers_outfile",
    Datatype.filePath,
    defaultValue: "hidden_layers.out",
    description: "Output hidden layers file name (default: hidden_layers.out).",
  ),
  Field(
    "Whether to extract rules with dimlpBT algorithm",
    "with_rule_extraction",
    Datatype.boolean,
    description: "Whether to extract rules with dimlpBT algorithm.",
  ),
  Field(
    "Rules output file",
    "global_rules_outfile",
    Datatype.filePath,
    description: "Rules output file.",
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
    "Back-propagation flat spot elimination parameter",
    "flat",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "0.01",
    description: "Back-propagation flat spot elimination parameter.",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    maxValue: "inf",
    defaultValue: "50",
    description: "Number of stairs in staircase activation function.",
  ),
  Field(
    "Error threshold to stop training",
    "error_thresh",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "inf",
    description: "Error threshold to stop training.",
  ),
  Field(
    "Accuracy threshold to stop training",
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
        "Absolute difference error threshold, 0 if not using this stopping criteria (default: 0).",
  ),
  Field(
    "Number of epochs",
    "nb_epochs",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description: "Number of epochs (default: 1500).",
  ),
  Field(
    "Number of epochs to show error",
    "nb_epochs_error",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description: "Number of epochs to show error (default: 10).",
  ),
  Field(
    "Number of examples for one single network",
    "nb_ex_per_net",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    description:
        "Number of examples for one single network, 0 for all examples, it is not recommended to change this value (default: 0).",
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
    "--test_data_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing the test portion of the dataset.",
  ),
  Field(
    "Weights file",
    "--weights_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing weights.",
  ),
  Field(
    "Number of attributes",
    "--nb_attributes",
    Datatype.integer,
    isRequired: true,
    minValue: "1",
    maxValue: "inf",
    description: "Number of input neurons.",
  ),
  Field(
    "Number of classes",
    "--nb_classes",
    Datatype.integer,
    isRequired: true,
    minValue: "2",
    maxValue: "inf",
    description: "Number of output neurons.",
  ),
  Field(
    "Hidden layers file",
    "--hidden_layers_file",
    Datatype.filePath,
    isRequired: true,
    description: "Hidden layers file name.",
  ),
  Field(
    "Test true class file",
    "--test_class_file",
    Datatype.filePath,
    description:
        "Path of file containing true classes for the test portion of the dataset.",
  ),
  Field(
    "Tests prediction output file",
    "--test_pred_outfile",
    Datatype.filePath,
    defaultValue: "dimlpTest.out",
    description:
        "Path of output file containing predictions for the tests portion of the dataset.",
  ),
  consoleFileFld,
  Field(
    "Stats file",
    "--stats_file",
    Datatype.filePath,
    description: "Path of output file name containing statistics.",
  ),
  Field(
    "First hidden layer values file",
    "--hid_file",
    Datatype.filePath,
    defaultValue: "dimlpTest.hid",
    description: "Path of output file with first hidden layer values.",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "--nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    maxValue: "inf",
    defaultValue: "50",
    description: "Number of stairs in staircase activation function.",
  ),
];

// TODO Python forms
// TODO adapt to dropdown menu if needed
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
    description: "Whether to show parameters (default: True)",
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
    description: "Output ROC curve file name (default: roc_curve.png)",
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
    description: "Percentage of train data taken for validation (default: 0.1)",
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
        "Whether input image datas are normalized between 0 and 1 (default: False, default if with_hsl: True)",
  ),
  Field(
    "Number of training epochs",
    "nb_epochs",
    Datatype.integer,
    minValue: "1",
    defaultValue: "80",
    description: "Number of training epochs (default: 80)",
  ),
  Field(
    "Output train and validation (in this order) prediction file name",
    "train_valid_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTrain.out",
    description:
        "Output train and validation (in this order) prediction file name (default: predTrain.out)",
  ),
  Field(
    "Output test prediction file name",
    "test_pred_outfile",
    Datatype.filePath,
    defaultValue: "predTest.out",
    description: "Output test prediction file name (default: predTest.out)",
  ),
  Field(
    "Output weights file name",
    "weights_outfile",
    Datatype.filePath,
    defaultValue: "weights.wts",
    description: "Output weights file name (default: weights.wts)",
  ),
  Field(
    "Output statistic file name with train and test accuracy",
    "stats_file",
    Datatype.filePath,
    defaultValue: "stats.txt",
    description:
        "Output statistic file name with train and test accuracy (default: stats.txt)",
  ),
  consoleFileFld,
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    description:
        "Number of stairs in staircase activation function (default: 50)",
  ),
  Field(
    "Parameter to improve dynamics",
    "K",
    Datatype.doublePrecision,
    minValue: "0",
    description: "Parameter to improve dynamics (default: 1.0)",
  ),
  Field(
    "Whether to change 3-channels data from RGB to HSL format",
    "with_hsl",
    Datatype.boolean,
    description:
        "Whether to change 3-channels data from RGB to HSL format (default: False)",
  ),
  Field(
    "Whether to train with ResNet",
    "with_resnet",
    Datatype.boolean,
    description: "Whether to train with ResNet (default: False)",
  ),
  Field(
    "Whether to train with VGG",
    "with_vgg",
    Datatype.boolean,
    description: "Whether to train with VGG (default: False)",
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
const List<Field> mlpTrnFields = [];
const List<Field> randForestsTrnFields = [];
const List<Field> svmTrnFields = [];
