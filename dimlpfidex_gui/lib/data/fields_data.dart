import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/data/common_fields_data.dart';

// TODO: complete datas (must have)
// TODO: some descriptions are not sufficient

const List<Field> fidexFields = [
  rootFolderFld,
  trainDataFileFld,
  trainPredFileFld,
  Field(
    "Train true class file",
    "--train_class_file",
    Datatype.filePath,
    description:
        "Train true class file, not mandatory if classes are specified in train data file",
  ),
  Field(
    "Test sample(s) data file",
    "--test_data_file",
    Datatype.filePath,
    isRequired: true,
    description:
        "Test sample(s) data file with data, prediction (if no --test_pred_file) and true class (if no --test_class_file)",
  ),
  Field(
    "Weights file",
    "--weights_file",
    Datatype.filePath,
    description:
        "Weights file (not mandatory if a rules file is given with --rules_file)",
  ),
  Field(
    "Rules file",
    "--rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)",
  ),
  Field(
    "Rule(s) output file",
    "--rules_outfile",
    Datatype.filePath,
    description: "Rule(s) output file",
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "JSON file to configure all parameters",
    "--json_config_file",
    Datatype.filePath,
    description:
        "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path",
  ),
  Field(
    "Folder based on main folder dimlpfidex",
    "--root_folder",
    Datatype.directoryPath,
    description:
        "Folder based on main folder dimlpfidex(default folder) containing all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder",
  ),
  Field(
    "Test prediction file",
    "--test_pred_file",
    Datatype.filePath,
    description: "Test prediction file",
  ),
  Field(
    "Test true class file",
    "--test_class_file",
    Datatype.filePath,
    description:
        "Test true class file. If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case)",
  ),
  Field(
    "File of attributes",
    "--attributes_file",
    Datatype.filePath,
    description: "File of attributes",
  ),
  Field(
    "Output statistic file name",
    "--stats_file",
    Datatype.filePath,
    description: "Output statistic file name",
  ),
  Field(
    "File with console logs redirection",
    "--console_file",
    Datatype.filePath,
    description: "File with console logs redirection",
  ),
  Field(
    "Max iteration number",
    "--max_iterations",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "10",
    description:
        "Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images.",
  ),
  Field(
    "Minimum covering number",
    "--min_covering",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "2",
    description: "Minimum covering number.",
  ),
  Field(
    "Whether to use this strategy",
    "--covering_strategy",
    Datatype.boolean,
    description:
        "Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)",
  ),
  Field(
    "Maximum number of failed attempts",
    "--max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "30",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)",
  ),
  Field(
    "Minimal rule fidelity accepted",
    "--min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "1.0",
    description:
        "Minimal rule fidelity accepted when generating a rule (default: 1.0)",
  ),
  Field(
    "Minimal min_fidelity",
    "--lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.75",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)",
  ),
  Field(
    "Dimension dropout parameter",
    "--dropout_dim",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.0",
    description: "Dimension dropout parameter (default: 0.0)",
  ),
  Field(
    "Hyperplan dropout parameter",
    "--dropout_hyp",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.0",
    description: "Hyperplan dropout parameter (default: 0.0)",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "--nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    maxValue: "inf",
    defaultValue: "50",
    description:
        "Number of stairs in staircase activation function (default: 50)",
  ),
  Field(
    "Decision threshold for predictions",
    "--decision_threshold",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Decision threshold for predictions, you need to specify the index of positive class if you want to use it",
  ),
  Field(
    "Index of positive class",
    "--positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "nb_classes - 1",
    description:
        "Index of positive class for the usage of decision threshold, index starts at 0",
  ),
  Field(
    "File containing the mean and std",
    "--normalization_file",
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
    "--test_data_file",
    Datatype.filePath,
    description:
        "Test sample(s) data file with data and prediction. If no --test_pred_file is specified, classes may be included here if launching with fidex(--with_fidex).",
  ),
  Field(
    "Ruleset input file",
    "--global_rules_file",
    Datatype.filePath,
    description: "Ruleset input file.",
  ),
  nbAttributesFld,
  nbClassesFld,
  Field(
    "File of attributes",
    "--attributes_file",
    Datatype.filePath,
    description:
        "File of attributes. Mandatory if rules file contains attribute names, if not, do not add it.",
  ),
  Field(
    "Test prediction file",
    "--test_pred_file",
    Datatype.filePath,
    description:
        "Test prediction file. If given, --test_data_file needs to have only test data.",
  ),
  Field(
    "Explanation(s) output file name",
    "--explanation_file",
    Datatype.filePath,
    description: "Explanation(s) output file name.",
  ),
  Field(
    "File with console logs redirection",
    "--console_file",
    Datatype.filePath,
    description: "File with console logs redirection.",
  ),
  Field(
    "Decision threshold for predictions",
    "--decision_threshold",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Decision threshold for predictions. You need to specify the index of the positive class if you want to use it.",
  ),
  Field(
    "Index of positive class",
    "--positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "# classes - 1",
    description:
        "Index of positive class for the usage of decision threshold. Index starts at 0.",
  ),
  Field(
    "Use fidex algorithm",
    "--with_fidex",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to use Fidex algorithm if no rule is found in global rules.",
  ),
  Field(
    "Use minimal version",
    "--with_minimal_version",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to use minimal version, which only gets correct activated rules and if with_fidex, launches Fidex when no such rule is found.",
  ),
  trainDataFileFld,
  trainPredFileFld,
  Field(
    "Train true class file",
    "--train_class_file",
    Datatype.filePath,
    description:
        "Train true class file. Not mandatory if classes are specified in train data file.",
  ),
  Field(
    "Weights file",
    "--weights_file",
    Datatype.filePath,
    description:
        "Weights file. Not mandatory if a rules file is given with --rules_file.",
  ),
  Field(
    "Rules file",
    "--rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus. Not mandatory if a weights file is given with --weights_file.",
  ),
  Field(
    "Test true class file",
    "--test_class_file",
    Datatype.filePath,
    description:
        "Test true class file. Classes can be specified in the test data file.",
  ),
  Field(
    "Max iteration number",
    "--max_iterations",
    Datatype.integer,
    minValue: "1",
    defaultValue: "10",
    description:
        "Max iteration number, also the max possible number of attributes in a rule. Should be 25 if working with images (default: 10).",
  ),
  Field(
    "Minimum covering number",
    "--min_covering",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "2",
    description: "Minimum covering number.",
  ),
  Field(
    "Use covering stategy",
    "--covering_strategy",
    Datatype.boolean,
    defaultValue: "false",
    description:
        "Whether to use this strategy: if no rule is found with min_covering, find the best rule with the best covering using dichotomic search. Decreases min_fidelity if needed (default: True).",
  ),
  Field(
    "Maximum number of failed attempts to find Fidex rule",
    "--max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30).",
  ),
  Field(
    "Minimal rule fidelity accepted when generating a rule",
    "--min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Minimal rule fidelity accepted when generating a rule (default: 1.0).",
  ),
  Field(
    "Minimal min_fidelity to which we agree to go down during covering_strategy",
    "--lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75).",
  ),
  Field(
    "Dimension dropout parameter",
    "--dropout_dim",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description: "Dimension dropout parameter (default: 0.0).",
  ),
  Field(
    "Hyperplan dropout parameter",
    "--dropout_hyp",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description: "Hyperplan dropout parameter (default: 0.0).",
  ),
  Field(
    "Number of stairs in staircase activation function",
    "--nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    description:
        "Number of stairs in staircase activation function (default: 50).",
  ),
  Field(
    "File containing the mean and std of some attributes",
    "--normalization_file",
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
  Field(
    "Train data file",
    "--train_data_file",
    Datatype.filePath,
    description: "Train data file.",
  ),
  Field(
    "Train prediction file",
    "--train_pred_file",
    Datatype.filePath,
    description: "Train prediction file.",
  ),
  Field(
    "Train true class file",
    "--train_class_file",
    Datatype.filePath,
    description:
        "Train true class file, not mandatory if classes are specified in train data file.",
  ),
  Field(
    "Weights file",
    "--weights_file",
    Datatype.filePath,
    description:
        "Weights file (not mandatory if a rules file is given with --rules_file).",
  ),
  Field(
    "Rules file to be converted to hyperlocus",
    "--rules_file",
    Datatype.filePath,
    description:
        "Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file).",
  ),
  Field(
    "Rules output file",
    "--global_rules_outfile",
    Datatype.filePath,
    description: "Rules output file.",
    isRequired: true,
  ),
  Field(
    "Heuristic",
    "--heuristic",
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
    "JSON file to configure all parameters",
    "--json_config_file",
    Datatype.filePath,
    description:
        "JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path.",
  ),
  Field(
    "File of attributes",
    "--attributes_file",
    Datatype.filePath,
    description: "File of attributes.",
  ),
  Field(
    "File with console logs redirection",
    "--console_file",
    Datatype.filePath,
    description: "File with console logs redirection.",
  ),
  Field(
    "Max iteration number",
    "--max_iterations",
    Datatype.integer,
    minValue: "1",
    defaultValue: "10",
    description:
        "Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images.",
  ),
  Field(
    "Minimum covering number",
    "--min_covering",
    Datatype.integer,
    minValue: "1",
    maxValue: "inf",
    defaultValue: "2",
    description: "Minimum covering number.",
  ),
  Field(
    "Whether to use this strategy",
    "--covering_strategy",
    Datatype.boolean,
    description:
        "Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True).",
  ),
  Field(
    "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used",
    "--max_failed_attempts",
    Datatype.integer,
    minValue: "0",
    maxValue: "inf",
    defaultValue: "30",
    description:
        "Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used.",
  ),
  Field(
    "Minimal rule fidelity accepted when generating a rule",
    "--min_fidelity",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "1.0",
    description: "Minimal rule fidelity accepted when generating a rule.",
  ),
  Field(
    "Minimal min_fidelity to which we agree to go down during covering_strategy",
    "--lowest_min_fidelity",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    defaultValue: "0.75",
    description:
        "Minimal min_fidelity to which we agree to go down during covering_strategy.",
  ),
  Field(
    "Dimension dropout parameter",
    "--dropout_dim",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "0.0",
    description: "Dimension dropout parameter.",
  ),
  Field(
    "Hyperplan dropout parameter",
    "--dropout_hyp",
    Datatype.doublePrecision,
    minValue: "0.0",
    maxValue: "1.0",
    defaultValue: "0.0",
    description: "Hyperplan dropout parameter.",
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
  Field(
    "Decision threshold for predictions",
    "--decision_threshold",
    Datatype.doublePrecision,
    minValue: "0",
    maxValue: "1",
    description:
        "Decision threshold for predictions, you need to specify the index of positive class if you want to use it.",
  ),
  Field(
    "Index of positive class for the usage of decision threshold",
    "--positive_class_index",
    Datatype.integer,
    minValue: "0",
    maxValue: "# classes - 1",
    description:
        "Index of positive class for the usage of decision threshold, index starts at 0.",
  ),
  Field(
    "File containing the mean and std of some attributes",
    "--normalization_file",
    Datatype.filePath,
    description:
        "File containing the mean and std of some attributes. Used to denormalize the rules if specified.",
  ),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  Field(
    "Number of threads used for computing the algorithm",
    "--nb_threads",
    Datatype.integer,
    minValue: "1",
    maxValue: "# CPU cores",
    description:
        "Number of threads used for computing the algorithm, 1=sequential execution.",
  ),
  seedFld
];

const List<Field> fidexGloStatsFields = [
  Field(
    "Test data file",
    "test_data_file",
    Datatype.string,
    isRequired: true,
  ),
  Field(
    "Test prediction file",
    "test_pred_file",
    Datatype.string,
    isRequired: true,
  ),
  Field(
    "Test class file",
    "test_class_file",
    Datatype.string,
    description: "Not mendatory if classes are specified in the test data file",
  ),
  Field(
    "Global rules input file",
    "global_rules_file",
    Datatype.string,
    isRequired: true,
  ),
  Field(
    "Number of attributes",
    "nb_attributes",
    Datatype.integer,
    isRequired: true,
  ),
  Field(
    "Number of classes",
    "nb_classes",
    Datatype.integer,
    isRequired: true,
  ),
  // Options
  Field(
    "Root folder",
    "root_folder",
    Datatype.directoryPath,
    description:
        "Folder based on the main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder",
  ),
  Field(
    "Attributes file",
    "attributes_file",
    Datatype.string,
    description:
        "Mandatory if the rules file contains attribute names, if not, do not add it",
  ),
  Field("Stats output file", "stats_file", Datatype.string),
  Field(
    "Global rules output file with stats on the test set",
    "global_rules_outfile",
    Datatype.string,
    description:
        "If you want to compute statistics of global rules on the test set",
  ),
  Field(
    "Console file",
    "console_file",
    Datatype.string,
    description: "File where you redirect the terminal results if you want to",
  ),
  Field(
    "Decision threshold",
    "decision_threshold",
    Datatype.doublePrecision,
    description:
        "Decision threshold for predictions, use if it was used in FidexGlo, need to specify the index of the positive class if you want to use it (None by default)",
  ),
  Field(
    "Positive class index",
    "positive_class_index",
    Datatype.integer,
    description:
        "Index of the positive class sample to compute true/false positive/negative rates (None by default, put 0 for the first class)",
  ),
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
  Field(
    "Number of stairs in staircase activation function",
    "nb_quant_levels",
    Datatype.integer,
    minValue: "3",
    maxValue: "inf",
    defaultValue: "50",
    description: "Number of stairs in staircase activation function.",
  ),
];

const List<Field> dimlpTrnFields = [
  rootFolderFld,
  Field(
    "Train data file",
    "train_data_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing the train portion of the dataset.",
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
  Field(
    "Train data file",
    "train_data_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of file containing the train portion of the dataset.",
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
  Field(
    "Train data file",
    "train_data_file",
    Datatype.filePath,
    isRequired: true,
    description: "Path of train data file.",
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
    maxValue: "inf",
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



// TODO Python forms