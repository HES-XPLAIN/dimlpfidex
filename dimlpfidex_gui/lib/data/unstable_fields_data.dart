import 'package:dimlpfidex_gui/data/field.dart';
import 'package:dimlpfidex_gui/data/unstable_common_fields_data.dart';

const List<UnstableField> fidexFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train data file.",
    )
  ]),
  trainPredFileFld,
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test sample(s) data, prediction (if no test prediction file is specified) and true class (if no test true classes file is specified).",
    )
  ]),
  UnstableField("Weights file", "weights_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the trained weights of the model (mandatory if no rules file is given).",
    )
  ]),
  UnstableField("Rules file", "rules_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the trained rules to be converted to hyperlocus (mandatory if no weights file is given).",
    )
  ]),
  UnstableField("Rules output file", "rules_outfile", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the output rule(s) will be stored. If a .json extension is given, rules are saved in JSON format.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset. If at least the test prediction file is specified, the test data file needs to have only the test datas and eventually classes on same line (don't add the test true classes file in this case).",
    )
  ]),
  UnstableField("Test prediction file", "test_pred_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing predictions on the test portion of the dataset.",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where statistics concerning the algorithm execution will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField(
      "Maximum number of training iterations", "max_iterations", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "10",
      description:
          "Maximum training iteration number, also the maximum possible number of attributes in a rule, it should be 25 when working with images.",
    )
  ]),
  UnstableField("Minimum covering number", "min_covering", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "2",
      description: "Minimum number of samples covered by the generated rules.",
    )
  ]),
  UnstableField("Use covering strategy", "covering_strategy", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description:
          "Whether to use the covering strategy: if no rule is found with minimum covering, find best rule with best covering using dichotomic search. Decreases minimum fidelity if needed.",
    )
  ]),
  UnstableField("Maximum number of failed attempts to find a Fidex rule",
      "max_failed_attempts", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "30",
      description:
          "Maximum number of failed attempts to find a Fidex rule when the covering is 1 and the covering strategy is used.",
    )
  ]),
  UnstableField("Minimal rule fidelity accepted", "min_fidelity", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "1.0",
      description: "Minimal rule fidelity accepted when generating a rule.",
    )
  ]),
  UnstableField(
      "Lowest minimal rule fidelity accepted", "lowest_min_fidelity", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "0.75",
      description:
          "Minimal min_fidelity to which we agree to go down during the covering strategy.",
    )
  ]),
  dropoutDimFld,
  dropoutHypFld,
  UnstableField(
      "Decision threshold for predictions", "decision_threshold", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "The decision threshold used for predictions, you need to specify the index of the positive class if you want to use it.",
    )
  ]),
  UnstableField("Index of positive class", "positive_class_index", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "# classes - 1",
      description:
          "Index of the positive class for the usage of a decision threshold, index starts at 0.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld,
];

const List<UnstableField> fidexGloFields = [
  rootFolderFld,
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing test sample(s) data, prediction (if no test prediction file is specified) and true class if launching with fidex (and if no test true classes file is specified).",
    )
  ]),
  UnstableField("Global rules input file", "global_rules_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the global rules obtained with fidexGloRules algorithm.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Test prediction file", "test_pred_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing predictions on the test portion of the dataset. If given, test data file needs to have only the test data.",
    )
  ]),
  UnstableField("Explanation file", "explanation_file", false, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file where explanation(s) will be stored.",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes. Mandatory if rules file contains attribute names, if not, do not add it.",
    )
  ]),
  consoleFileFld,
  UnstableField("Use minimal version", "with_minimal_version", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to use minimal version, which only gets correct activated rules and if with_fidex, launches Fidex when no such rule is found.",
    )
  ]),
  UnstableField(
      "Use fidex algorithm (train data, prediction and class files, and either weights file or rules files are then mandatory)",
      "with_fidex",
      false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to use the Fidex algorithm if no rule is found in global rules.",
    )
  ]),
  UnstableField("Train data file", "train_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train portion of the dataset.",
    )
  ]),
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset. Mandatory if classes are not specified in train data file.",
    )
  ]),
  UnstableField("Train prediction file", "train_pred_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing predictions on the train portion of the dataset.",
    )
  ]),
  UnstableField("Weights file", "weights_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the trained weights of the model (mandatory if no rules file is given).",
    )
  ]),
  UnstableField("Rules file", "rules_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the trained rules to be converted to hyperlocus (mandatory if no weights file is given).",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset. Classes can be specified in the test data file.",
    )
  ]),
  UnstableField(
      "Maximum number of training iterations", "max_iterations", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "10",
      description:
          "Maximum training iteration number, also the maximum possible number of attributes in a rule, it should be 25 when working with images.",
    )
  ]),
  UnstableField("Minimum covering number", "min_covering", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "2",
      description: "Minimum number of samples covered by the generated rules.",
    )
  ]),
  UnstableField("Use covering strategy", "covering_strategy", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description:
          "Whether to use the covering strategy: if no rule is found with minimum covering, find best rule with best covering using dichotomic search. Decreases minimum fidelity if needed.",
    )
  ]),
  UnstableField("Maximum number of failed attempts to find a Fidex rule",
      "max_failed_attempts", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "30",
      description:
          "Maximum number of failed attempts to find a Fidex rule when the covering is 1 and the covering strategy is used.",
    )
  ]),
  UnstableField("Minimal rule fidelity accepted", "min_fidelity", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "1.0",
      description: "Minimal rule fidelity accepted when generating a rule.",
    )
  ]),
  UnstableField(
      "Lowest minimal rule fidelity accepted", "lowest_min_fidelity", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "Minimal min_fidelity to which we agree to go down during the covering strategy.",
    )
  ]),
  UnstableField("Number of Fidex rules to generate per sample with Fidex",
      "nb_fidex_rules", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "1",
      description:
          "Number of Fidex rules to compute per sample when launching the Fidex algorithm.",
    )
  ]),
  dropoutDimFld,
  dropoutHypFld,
  nbQuantLevelsFld,
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld,
];

const List<UnstableField> fidexGloRulesFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train data file.",
    )
  ]),
  trainPredFileFld,
  UnstableField("Weights file", "weights_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the trained weights of the model (mandatory if no rules file is given).",
    )
  ]),
  UnstableField("Rules file", "rules_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the trained rules to be converted to hyperlocus (mandatory if no weights file is given).",
    )
  ]),
  UnstableField("Global rules output file", "global_rules_outfile", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the output rule(s) will be stored. If a .json extension is given, rules are saved in JSON format.",
    )
  ]),
  UnstableField("Heuristic", "heuristic", true, [
    Metadata(
      Datatype.integer,
      description:
          "Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo. (Faster algorithms are less efficient).",
      minValue: "1",
      maxValue: "3",
      defaultValue: "1",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes.",
    )
  ]),
  consoleFileFld,
  UnstableField(
      "Maximum number of training iterations", "max_iterations", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "10",
      description:
          "Maximum training iteration number, also the maximum possible number of attributes in a rule, it should be 25 when working with images.",
    )
  ]),
  UnstableField("Minimum covering number", "min_covering", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "2",
      description: "Minimum number of samples covered by the generated rules.",
    )
  ]),
  UnstableField("Use covering strategy", "covering_strategy", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description:
          "Whether to use the covering strategy: if no rule is found with minimum covering, find best rule with best covering using dichotomic search. Decreases minimum fidelity if needed.",
    )
  ]),
  UnstableField("Maximum number of failed attempts to find a Fidex rule",
      "max_failed_attempts", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "30",
      description:
          "Maximum number of failed attempts to find a Fidex rule when the covering is 1 and the covering strategy is used.",
    )
  ]),
  UnstableField("Minimal rule fidelity accepted", "min_fidelity", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "1.0",
      description: "Minimal rule fidelity accepted when generating a rule.",
    )
  ]),
  UnstableField(
      "Lowest minimal rule fidelity accepted", "lowest_min_fidelity", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "0.75",
      description:
          "Minimal min_fidelity to which we agree to go down during the covering strategy.",
    )
  ]),
  dropoutDimFld,
  dropoutHypFld,
  UnstableField(
      "Decision threshold for predictions", "decision_threshold", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "The decision threshold used for predictions, you need to specify the index of the positive class if you want to use it.",
    )
  ]),
  UnstableField("Index of positive class", "positive_class_index", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "# classes - 1",
      description:
          "Index of the positive class for the usage of a decision threshold, index starts at 0.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  UnstableField("Number of threads used for computing the algorithm",
      "nb_threads", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "# CPU cores",
      description:
          "Number of threads used for computing the algorithm, 1=sequential execution.",
    )
  ]),
  seedFld
];

const List<UnstableField> fidexGloStatsFields = [
  rootFolderFld,
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test data file.",
    )
  ]),
  UnstableField("Test prediction file", "test_pred_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing predictions on the test portion of the dataset.",
    )
  ]),
  UnstableField("Global rules input file", "global_rules_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the global rules obtained with fidexGloRules algorithm.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output rules file", "global_rules_outfile", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the output global rules will be stored with stats on test set, if you want to compute those statistics.",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes. Mandatory if rules file contains attribute names, if not, do not add it.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where statistics of the global ruleset will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Index of positive class", "positive_class_index", false, [
    Metadata(
      Datatype.integer,
      description:
          "Index of positive class for the usage of a decision threshold and to compute true/false positive/negative rates, index starts at 0. If it is specified in the rules file, it has to be the same value.",
      minValue: "0",
      maxValue: "# classes - 1",
    )
  ]),
];

const List<UnstableField> dimlpTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in the train data file.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output train prediction file", "train_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpTrain.out",
      description:
          "Path to the file where the train predictions will be stored.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("Validation data file", "valid_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the validation portion of the dataset.",
    )
  ]),
  UnstableField("Validation true classes file", "valid_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the validation true classes of the dataset.",
    )
  ]),
  UnstableField(
      "Output validation prediction file", "valid_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpValidation.out",
      description:
          "Path to the file where the validation predictions will be stored.",
    )
  ]),
  UnstableField("Pretrained weights file", "weights_file", false, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file containing pretrained weights.",
    )
  ]),
  UnstableField("Output weights file", "weights_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlp.wts",
      description:
          "Path to the file where the output trained weights of the model will be stored.",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "statsDimlpTrn.txt",
      description:
          "Path to the file where the train, test and validation accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Number of neurons in the first hidden layer",
      "first_hidden_layer", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      description:
          "Number of neurons in the first hidden layer. It has to be a multiple of the number of attributes. The default value is the number of attributes.",
    )
  ]),
  UnstableField(
      "Number of neurons in each hidden layer", "hidden_layers", false, [
    Metadata(
      Datatype.listInteger,
      minValue: "1",
      maxValue: "inf",
      description:
          "Number of neurons in each hidden layer, from the second layer to the last.",
    )
  ]),
  UnstableField("Output hidden layers file", "hidden_layers_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "hidden_layers.out",
      description:
          "Path to the file where output hidden layers sizes will be stored.",
    )
  ]),
  UnstableField("Whether to extract rules with dimlp algorithm",
      "with_rule_extraction", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description: "Whether to extract rules with dimlp algorithm.",
    )
  ]),
  UnstableField("Global rules output file", "global_rules_outfile", false, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file where the output rule(s) will be stored.",
    )
  ]),
  UnstableField("Back-propagation learning rate", "learning_rate", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.1",
      description: "Back-propagation learning rate parameter.",
    )
  ]),
  UnstableField("Back-propagation momentum parameter", "momentum", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.6",
      description: "Back-propagation momentum parameter.",
    )
  ]),
  UnstableField(
      "Back-propagation flat spot elimination parameter", "flat", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.1",
      description: "Back-propagation flat spot elimination parameter.",
    )
  ]),
  UnstableField("Error threshold", "error_thresh", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      description: "Error threshold to stop training.",
    )
  ]),
  UnstableField("Accuracy threshold", "acc_thresh", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "1",
      description: "Accuracy threshold to stop training.",
    )
  ]),
  UnstableField(
      "Absolute difference error threshold", "abs_error_thresh", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0",
      description:
          "Absolute difference error threshold, 0 if not using this stopping criteria.",
    )
  ]),
  UnstableField("Number of model training epochs", "nb_epochs", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "1500",
      description: "Number of model training epochs.",
    )
  ]),
  UnstableField("Number of training epochs before showing error",
      "nb_epochs_error", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "10",
      description: "Number of training epochs before showing error.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld
];

const List<UnstableField> dimlpPredFields = [
  rootFolderFld,
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Weights file trained with dimlpTrn", "weights_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the weights of the model trained with dimlpTrn.",
    )
  ]),
  UnstableField("Hidden layers' sizes file", "hidden_layers_file", true, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file containing hidden layers sizes.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  consoleFileFld,
  nbQuantLevelsFld,
];

const List<UnstableField> dimlpRulFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train data file.",
    )
  ]),
  UnstableField("Weights file trained with dimlpTrn", "weights_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the weights of the model trained with dimlpTrn.",
    )
  ]),
  UnstableField("Hidden layers' sizes file", "hidden_layers_file", true, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file containing hidden layers sizes.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Test data file", "test_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset.",
    )
  ]),
  UnstableField("Validation data file", "valid_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the validation portion of the dataset.",
    )
  ]),
  UnstableField("Validation true classes file", "valid_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the validation true classes of the dataset.",
    )
  ]),
  UnstableField("Global rules output file", "global_rules_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlp.rls",
      description: "Path to the file where the output rule(s) will be stored.",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the train, test and validation accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  nbQuantLevelsFld,
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
];

const List<UnstableField> dimlpBTFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train data file.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Number of networks", "nb_dimlp_nets", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "25",
      minValue: "1",
      maxValue: "inf",
      description: "Number of networks.",
    )
  ]),
  UnstableField("Output train prediction file", "train_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpBTTrain.out",
      description:
          "Path to the file where the train predictions will be stored.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpBTTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("Output weights file", "weights_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpBT.wts",
      description:
          "Path to the file where the output trained weights of the model will be stored.",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "statsDimlpBT.txt",
      description:
          "Path to the file where the train, test and validation accuracy, as well as the train and test global accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Number of neurons in the first hidden layer",
      "first_hidden_layer", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      description:
          "Number of neurons in the first hidden layer. It has to be a multiple of the number of attributes. The default value is the number of attributes.",
    )
  ]),
  UnstableField(
      "Number of neurons in each hidden layer", "hidden_layers", false, [
    Metadata(
      Datatype.listInteger,
      minValue: "1",
      maxValue: "inf",
      description:
          "Number of neurons in each hidden layer, from the second layer to the last.",
    )
  ]),
  UnstableField("Output hidden layers file", "hidden_layers_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "hidden_layers.out",
      description:
          "Path to the file where output hidden layers sizes will be stored.",
    )
  ]),
  UnstableField("Whether to extract rules with dimlpBT algorithm",
      "with_rule_extraction", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description: "Whether to extract rules with dimlpBT algorithm.",
    )
  ]),
  UnstableField("Global rules output file", "global_rules_outfile", false, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file where the output rule(s) will be stored.",
    )
  ]),
  UnstableField("Back-propagation momentum parameter", "momentum", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.6",
      description: "Back-propagation momentum parameter.",
    )
  ]),
  UnstableField(
      "Back-propagation flat spot elimination parameter", "flat", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.01",
      description: "Back-propagation flat spot elimination parameter.",
    )
  ]),
  UnstableField("Error threshold", "error_thresh", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      description: "Error threshold to stop training.",
    )
  ]),
  UnstableField("Accuracy threshold", "acc_thresh", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "1",
      description: "Accuracy threshold to stop training.",
    )
  ]),
  UnstableField(
      "Absolute difference error threshold", "abs_error_thresh", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0",
      description:
          "Absolute difference error threshold, 0 if not using this stopping criteria.",
    )
  ]),
  UnstableField("Number of model training epochs", "nb_epochs", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "1500",
      description: "Number of model training epochs.",
    )
  ]),
  UnstableField("Number of training epochs before showing error",
      "nb_epochs_error", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "10",
      description: "Number of training epochs before showing error.",
    )
  ]),
  UnstableField(
      "Number of examples for one single network", "nb_ex_per_net", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "0",
      minValue: "1",
      maxValue: "inf",
      description:
          "Number of examples for one single network, 0 for all examples, it is not recommended to change this value.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
  seedFld,
];

const List<UnstableField> densClsFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train data file.",
    )
  ]),
  UnstableField("Weights file trained with dimlpBT", "weights_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the weights of the model trained with dimlpBT.",
    )
  ]),
  UnstableField("Hidden layers' sizes file", "hidden_layers_file", true, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file containing hidden layers sizes.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output train prediction file", "train_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "densClsTrain.out",
      description:
          "Path to the file where the train predictions will be stored.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "densClsTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the global train and test accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Whether to extract rules with dimlpBT algorithm",
      "with_rule_extraction", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description: "Whether to extract rules with dimlpBT algorithm.",
    )
  ]),
  UnstableField("Global rules output file", "global_rules_outfile", false, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file where the output rule(s) will be stored.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  musFld,
  sigmasFld,
  normalizationIndicesFld,
];

const List<UnstableField> dimlpClsFields = [
  rootFolderFld,
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test data file.",
    )
  ]),
  UnstableField("Weights file trained with dimlpTrn", "weights_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the weights of the model trained with dimlpTrn.",
    )
  ]),
  UnstableField("Hidden layers' sizes file", "hidden_layers_file", true, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file containing hidden layers sizes.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("First hidden layer values file", "hid_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "dimlpTest.hid",
      description:
          "Path to the file where the first hidden layer values will be stored.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file where the test accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  nbQuantLevelsFld,
];

const List<UnstableField> computeRocCurveFields = [
  rootFolderFld,
  UnstableField("Test true classes file", "test_class_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset.",
    )
  ]),
  UnstableField("Test prediction file", "test_pred_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing predictions on the test portion of the dataset.",
    )
  ]),
  UnstableField("Index of positive class", "positive_class_index", true, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "nb_classes-1",
      description: "Index of the positive class, index starts at 0.",
    )
  ]),
  nbClassesFld,
  UnstableField("Output ROC curve file", "output_roc", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "roc_curve.png",
      description: "Path to the file where the output ROC curve will be saved.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the AUC score will be added, it can be the training stats file.",
    )
  ]),
  UnstableField("Name of the estimator", "estimator", false, [
    Metadata(
      Datatype.string,
      description: "Name of the estimator.",
    )
  ]),
  UnstableField("Show parameters", "show_params", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description: "Whether to show the parameters.",
    )
  ]),
];

const List<UnstableField> cnnTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train_data_file.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test_data_file.",
    )
  ]),
  UnstableField("Original input size", "original_input_size", true, [
    Metadata(
      Datatype.pairInteger,
      minValue: "1",
      maxValue: "inf",
      description: "Original input size.",
    )
  ]),
  UnstableField("Number of channels in the input", "nb_channels", true, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      description:
          "Number of channels in the input (3 for RGB image, 1 for B&W image).",
    )
  ]),
  UnstableField("Model", "model", true, [
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["small", "large", "vgg", "resnet"],
      description: "Training model.",
    )
  ]),
  UnstableField("Data format", "data_format", true, [
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["normalized_01", "classic", "other"],
      description:
          "Format of the values of the data, normalized_01 if the data are normalized between 0 and 1, classic if they are between 0 and 255.",
    )
  ]),
  nbClassesFld,
  UnstableField("Output train and validation prediction file",
      "train_valid_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTrain.out",
      description:
          "Path to the file where the output train and validation (in this order) prediction will be stored.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField(
      "Percentage of train data taken for validation", "valid_ratio", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "1",
      defaultValue: "0.1",
      description: "Percentage of train data taken for validation.",
    )
  ]),
  UnstableField("Validation data file", "valid_data_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the validation portion of the dataset.",
    )
  ]),
  UnstableField("Validation true classes file", "valid_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the validation true classes of the dataset, mandatory if classes are not specified in valid_data_file. BE CAREFUL if there is validation files, and you want to use Fidex algorithms later, you will have to use both train and validation datas given here in the train datas and classes of Fidex.",
    )
  ]),
  UnstableField("Output weights file", "weights_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "weights.wts",
      description:
          "Path to the file where the output trained weights of the model will be stored.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "stats.txt",
      description:
          "Path to the file where the train and test accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Number of model training epochs", "nb_epochs", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "80",
      description: "Number of model training epochs.",
    )
  ]),
  UnstableField("Input size in the model", "model_input_size", false, [
    Metadata(
      Datatype.pairInteger,
      minValue: "1",
      maxValue: "inf",
      description:
          "Input size in the model. A small size is recommended to speed up the process. The size is modified if necessary.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField(
      "Parameter to improve dynamics by normalizing input data", "K", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "1.0",
      description: "Parameter to improve dynamics by normalizing input data.",
    )
  ]),
  seedFld,
];

const List<UnstableField> gradBoostTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train_data_file.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test_data_file.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output train prediction file", "train_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTrain.out",
      description:
          "Path to the file where the train predictions will be stored.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "stats.txt",
      description:
          "Path to the file where the train and test accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Rules output file", "rules_outfile", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the gradient boosting output rules will be stored.",
    )
  ]),
  UnstableField("Number of generated trees", "n_estimators", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      defaultValue: "100",
      description: "Number of generated trees in the forest.",
    )
  ]),
  UnstableField("Loss function", "loss", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "log_loss",
      items: ["log_loss", "exponential"],
      description: "Loss function to be optimized.",
    )
  ]),
  UnstableField("Learning rate", "learning_rate", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.1",
      description: "Shrinks the contribution of each tree.",
    )
  ]),
  UnstableField("Fraction of samples to be used", "subsample", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "1.0",
      description:
          "Fraction of samples to be used for fitting the individual base learners.",
    )
  ]),
  UnstableField("Function to measure split quality", "criterion", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "friedman_mse",
      items: ["friedman_mse", "squared_error"],
      description: "Function to measure split quality.",
    )
  ]),
  UnstableField("Maximum depth", "max_depth", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "3",
      description: "Maximum depth of the individual regression estimators.",
    ),
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["", "no_max_depth"],
      description: "Maximum depth of the individual regression estimators.",
    )
  ]),
  UnstableField(
      "Minimum number of samples to split a node", "min_samples_split", false, [
    Metadata(
      Datatype.integer,
      minValue: "2",
      maxValue: "inf",
      defaultValue: "2",
      description:
          "Minimum number of samples required to split an internal node.",
    ),
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "A fraction of the number of samples required to split an internal node.",
    ),
  ]),
  UnstableField(
      "Minimum number of samples at leaf node", "min_samples_leaf", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "1",
      minValue: "1",
      maxValue: "inf",
      description: "Minimum number of samples required to be at a leaf node.",
    ),
    Metadata(
      Datatype.doublePrecision,
      maxValue: "1.0",
      description:
          "A fraction of the number of samples required to be at a leaf node.",
    )
  ]),
  UnstableField(
      "Minimum weighted fraction", "min_weight_fraction_leaf", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "0.5",
      defaultValue: "0.0",
      description:
          "Minimum weighted fraction of the sum total of input samples weights required to be at a leaf node.",
    )
  ]),
  UnstableField("Maximum number of features", "max_features", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "sqrt",
      items: ["sqrt", "log2", "all"],
      description:
          "Number of features to consider when looking for the best split if float, it is a fraction of the number of features. 1 stands for 1 feature, for all features put 'all', not 1.0.",
    ),
    Metadata(
      Datatype.doublePrecision,
      description:
          "A fraction of the number of features to consider when looking for the best split. 1 stands for 1 feature, for all features put 'all', not 1.0.",
    )
  ]),
  UnstableField("Maximum leaf nodes", "max_leaf_nodes", false, [
    Metadata(
      Datatype.integer,
      minValue: "2",
      maxValue: "inf",
      description: "Grow trees with max_leaf_nodes in best-first fashion.",
    )
  ]),
  UnstableField("Minimum impurity decrease", "min_impurity_decrease", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "0.0",
      description:
          "A node will be split if this split induces a decrease of the impurity greater than or equal to this value.",
    )
  ]),
  UnstableField(
      "Estimator used to compute the initial predictions", "init", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["zero"],
      description: "Estimator object used to compute the initial predictions.",
    )
  ]),
  UnstableField("Seed for random number generation", "seed", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      description: "Seed for random number generation.",
    )
  ]),
  UnstableField("Verbosity level", "verbose", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0",
      description: "Controls the verbosity when fitting and predicting.",
    )
  ]),
  UnstableField(
      "Reuse the solution of the previous call", "warm_start", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to reuse the solution of the previous call to fit and add more estimators to the ensemble.",
    )
  ]),
  UnstableField("Proportion of training data", "validation_fraction", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "0.1",
      description:
          "Proportion of training data to set aside as validation set for early stopping.",
    )
  ]),
  UnstableField(
      "Number of iterations with no improvements", "n_iter_no_change", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      description:
          "Decide if early stopping will be used to terminate training when validation score is not improving, stopping if validation doesn't improve during this number of iterations.",
    )
  ]),
  UnstableField("Tolerance for the early stopping", "tol", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.0001",
      description: "Tolerance for the early stopping.",
    )
  ]),
  UnstableField("Complexity parameter", "ccp_alpha", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "0.0",
      description:
          "Complexity parameter used for Minimal Cost-Complexity Pruning.",
    )
  ]),
];

const List<UnstableField> mlpTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train_data_file.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test_data_file.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output train prediction file", "train_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTrain.out",
      description:
          "Path to the file where the train predictions will be stored.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "stats.txt",
      description:
          "Path to the file where the train and test accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Output weights file", "weights_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "weights.wts",
      description:
          "Path to the file where the output trained weights of the model will be stored.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField(
      "Parameter to improve dynamics by normalizing input data", "K", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "1.0",
      description: "Parameter to improve dynamics by normalizing input data.",
    )
  ]),
  UnstableField("Size of each hidden layers", "hidden_layer_sizes", false, [
    Metadata(
      Datatype.listInteger,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "100",
      description: "Size of each hidden layers.",
    )
  ]),
  UnstableField("Activation function", "activation", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["relu", "identity", "tanh", "logistic"],
      defaultValue: "relu",
      description: "Activation function.",
    )
  ]),
  UnstableField("Solver for weight optimization", "solver", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "adam",
      items: ["adam", "lbfgs", "sgd"],
      description: "Solver for weight optimization.",
    )
  ]),
  UnstableField("Strength of the L2 regularization term", "alpha", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "0.0001",
      description: "Strength of the L2 regularization term.",
    )
  ]),
  UnstableField("Size of minibatches", "batch_size", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      description:
          "Size of minibatches for stochastic optimizers for adam and stochastic gradient descent.",
    ),
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "auto",
      items: ["auto"],
      description:
          "Size of minibatches for stochastic optimizers for adam and stochastic gradient descent.",
    )
  ]),
  UnstableField("Learning rate schedule", "learning_rate", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "constant",
      items: ["constant", "invscalling", "adaptative"],
      description:
          "Learning rate schedule for weight updates for stochastic gradient descent solver.",
    )
  ]),
  UnstableField(
      "Initial learning rate for adam and stochastic gradient descent",
      "learning_rate_init",
      false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "0.001",
      description:
          "Initial learning rate for adam and stochastic gradient descent.",
    )
  ]),
  UnstableField(
      "Exponent for inverse scaling learning rate", "power_t", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "0.5",
      description:
          "Exponent for inverse scaling learning rate for stochastic gradient descent.",
    )
  ]),
  UnstableField(
      "Maximum number of training iterations", "max_iterations", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "200",
      description: "Maximum number of training iterations.",
    )
  ]),
  UnstableField("Shuffle samples", "shuffle", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description:
          "Whether to shuffle samples in each iteration for stochastic gradient descent and adam.",
    )
  ]),
  UnstableField("Seed for random number generation", "seed", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      description:
          "Seed for random number generation for stochastic gradient descent and adam.",
    )
  ]),
  UnstableField("Tolerance for optimization", "tol", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "0.0001",
      description: "Tolerance for optimization.",
    )
  ]),
  UnstableField("Enable verbose output", "verbose", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description: "Enable verbose output.",
    )
  ]),
  UnstableField("Reuse the previous solution to fit initialization",
      "warm_start", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to reuse the previous solution to fit initialization.",
    )
  ]),
  UnstableField("Momentum for gradient descent", "momentum", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "0.9",
      description:
          "Momentum for gradient descent update for stochastic gradient descent.",
    )
  ]),
  UnstableField("Use the Nesterov’s momentum", "nesterovs_momentum", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description:
          "Whether to use the Nesterov’s momentum for stochastic gradient descent and momentum.",
    )
  ]),
  UnstableField("Use early stopping", "early_stopping", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to use early stopping to terminate training when validation score is not improving for stochastic gradient descent and adam.",
    )
  ]),
  UnstableField("Proportion of training data to set aside as validation set",
      "validation_fraction", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "0.1",
      description:
          "Proportion of training data to set aside as validation set for early stopping.",
    )
  ]),
  UnstableField("First exponential decay rate", "beta_1", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "0.9",
      description:
          "Exponential decay rate for estimates of first moment vector in adam.",
    )
  ]),
  UnstableField("Second exponential decay rate", "beta_2", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      defaultValue: "0.999",
      description:
          "Exponential decay rate for estimates of second moment vector in adam.",
    )
  ]),
  UnstableField("Value for numerical stability", "epsilon", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "1e-08",
      description: "Value for numerical stability in adam.",
    )
  ]),
  UnstableField("Maximum number of epochs", "n_iter_no_change", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "10",
      description:
          "Maximum number of epochs to not meet tol improvement for stochastic gradient descent and adam.",
    )
  ]),
  UnstableField(
      "Maximum number of loss function calls for lbfgs", "max_fun", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      defaultValue: "15000",
      description: "Maximum number of loss function calls for lbfgs.",
    )
  ]),
];

const List<UnstableField> randForestsTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train_data_file.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test_data_file.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output train prediction file", "train_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTrain.out",
      description:
          "Path to the file where the train predictions will be stored.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "stats.txt",
      description:
          "Path to the file where the train and test accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Rules output file", "rules_outfile", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file where the random forests output rules will be stored.",
    )
  ]),
  UnstableField(
      "Number of generated trees in the forest", "n_estimators", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "100",
      minValue: "1",
      maxValue: "inf",
      description: "Number of generated trees in the forest.",
    )
  ]),
  UnstableField("Function to measure split quality", "criterion", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["gini", "entropy", "log_loss"],
      defaultValue: "gini",
      description: "Function to measure split quality.",
    )
  ]),
  UnstableField("Maximum depth of the tree", "max_depth", false, [
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      description: "Maximum depth of the tree.",
    )
  ]),
  UnstableField("Minimum number of samples to split an internal node",
      "min_samples_split", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "2",
      minValue: "2",
      maxValue: "inf",
      description:
          "Minimum number of samples required to split an internal node, if float, it is a fraction of the number of samples.",
    ),
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "A fraction of the number of samples required to split an internal node.",
    )
  ]),
  UnstableField(
      "Minimum number of samples to be at a leaf", "min_samples_leaf", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "1",
      minValue: "1",
      maxValue: "inf",
      description: "Minimum number of samples required to be at a leaf node.",
    ),
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "A fraction of the number of samples required to be at a leaf node.",
    )
  ]),
  UnstableField(
      "Minimum weighted fraction", "min_weight_fraction_leaf", false, [
    Metadata(
      Datatype.doublePrecision,
      defaultValue: "0.0",
      minValue: "0.0",
      maxValue: "0.5",
      description:
          "Minimum weighted fraction of the sum total of input samples weights required to be at a leaf node.",
    )
  ]),
  UnstableField(
      "Number of features to consider when looking for the best split",
      "max_features",
      false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "sqrt",
      items: ["sqrt", "log2", "all"],
      description:
          "Number of features to consider when looking for the best split.",
    ),
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "A fraction of the number of features to consider when looking for the best split. For all features put 'all', not 1.0.",
    ),
    Metadata(
      Datatype.integer,
      minValue: "1",
      maxValue: "inf",
      description:
          "Number of features to consider when looking for the best split. 1 stand for 1 feature.",
    )
  ]),
  UnstableField("Maximum number of leaf nodes", "max_leaf_nodes", false, [
    Metadata(
      minValue: "2",
      maxValue: "inf",
      Datatype.integer,
      description: "Grow trees with max_leaf_nodes in best-first fashion.",
    )
  ]),
  UnstableField("Minimal impurity decrease", "min_impurity_decrease", false, [
    Metadata(
      Datatype.doublePrecision,
      defaultValue: "0.0",
      minValue: "0.0",
      maxValue: "inf",
      description:
          "A node will be split if this split induces a decrease of the impurity greater than or equal to this value.",
    )
  ]),
  UnstableField("Use bootstrap samples", "bootstrap", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description: "Whether bootstrap samples are used when building trees.",
    )
  ]),
  UnstableField("Whether to use out-of-bag samples", "oob_score", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to use out-of-bag samples to estimate the generalization score.",
    )
  ]),
  UnstableField("Number of parallel jobs", "n_jobs", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "1",
      minValue: "-1",
      maxValue: "# CPUs available",
      description:
          "Number of jobs to run in parallel, -1 = using all processors.",
    )
  ]),
  UnstableField("Seed for random number generation", "seed", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      description: "Seed for random number generation.",
    )
  ]),
  UnstableField("Verbose level", "verbose", false, [
    Metadata(
      Datatype.integer,
      defaultValue: "0",
      minValue: "0",
      maxValue: "inf",
      description: "Controls the verbosity when fitting and predicting.",
    )
  ]),
  UnstableField(
      "Reuse the solution of the previous call", "warm_start", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to reuse the solution of the previous call to fit and add more estimators to the ensemble.",
    )
  ]),
  UnstableField("Class balance", "class_weight", false, [
    Metadata(
      Datatype.dictionary,
      description:
          "Class balance, for example with a dictionary and 2 classes: {0:1.2, 1:3.5}.",
    ),
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["balanced", "balanced_subsample"],
      description: "Class balance, can be 'balanced' and 'balanced_subsample'.",
    )
  ]),
  UnstableField("Complexity parameter", "ccp_alpha", false, [
    Metadata(
      Datatype.doublePrecision,
      defaultValue: "0.0",
      minValue: "0.0",
      maxValue: "inf",
      description:
          "Complexity parameter used for Minimal Cost-Complexity Pruning.",
    )
  ]),
  UnstableField("Maximum number of samples to draw", "max_samples", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "1.0",
      description:
          "A fraction of the number of samples to draw to train each base estimator for bootstrap.",
    ),
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      description:
          "Number of samples to draw to train each base estimator for bootstrap.",
    )
  ]),
];

const List<UnstableField> svmTrnFields = [
  rootFolderFld,
  trainDataFileFld,
  UnstableField("Train true classes file", "train_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train_data_file.",
    )
  ]),
  UnstableField("Test data file", "test_data_file", true, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test portion of the dataset.",
    )
  ]),
  UnstableField("Test true classes file", "test_class_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test_data_file.",
    )
  ]),
  nbAttributesFld,
  nbClassesFld,
  UnstableField("Output train prediction file", "train_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTrain.out",
      description:
          "Path to the file where the train predictions will be stored.",
    )
  ]),
  UnstableField("Output test prediction file", "test_pred_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "predTest.out",
      description:
          "Path to the file where the test predictions will be stored.",
    )
  ]),
  UnstableField("Output statistics file", "stats_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "stats.txt",
      description:
          "Path to the file where the train and test accuracy will be stored.",
    )
  ]),
  consoleFileFld,
  UnstableField("Output weights file", "weights_outfile", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "weights.wts",
      description:
          "Path to the file where the output trained weights of the model will be stored.",
    )
  ]),
  UnstableField("Output ROC curve file", "output_roc", false, [
    Metadata(
      Datatype.filePath,
      description: "Path to the file where the output ROC curve will be saved.",
    )
  ]),
  UnstableField("Whether to return ROC statistics", "return_roc", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description: "Whether to return ROC statistics.",
    )
  ]),
  UnstableField("Index of positive class", "positive_class_index", false, [
    Metadata(
      minValue: "0",
      maxValue: "# classes -1",
      Datatype.integer,
      description:
          "Index of the positive class for the roc curve calculation, index starts at 0.",
    )
  ]),
  nbQuantLevelsFld,
  UnstableField(
      "Parameter to improve dynamics by normalizing input data", "K", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "1.0",
      description: "Parameter to improve dynamics by normalizing input data.",
    )
  ]),
  UnstableField("Regularization parameter", "C", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "1.0",
      description: "Regularization parameter.",
    )
  ]),
  UnstableField("Kernel type", "kernel", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "rbf",
      items: ["linear", "poly", "rbf", "sigmoid"],
      description: "Kernel type.",
    )
  ]),
  UnstableField("Polynomial degree", "degree", false, [
    Metadata(
      Datatype.integer,
      minValue: "0",
      maxValue: "inf",
      defaultValue: "3",
      description: "Polynomial degree.",
    )
  ]),
  UnstableField(
      "Gamma predefined value.",
      "gamma",
      false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "scale",
      items: ["", "scale", "auto"],
      description: "Gamma predefined value.",
    ),
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      description: "Gamma custom floating point value.",
    )
  ]),
  UnstableField("Term in kernel function", "coef0", false, [
    Metadata(
      Datatype.doublePrecision,
      defaultValue: "0.0",
      description: "Term in the kernel function.",
    )
  ]),
  UnstableField("Use the shrinking heuristic", "shrinking", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description: "Whether to use the shrinking heuristic.",
    )
  ]),
  UnstableField("Tolerance for stopping criterion", "tol", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "0.001",
      description: "Tolerance for stopping criterion.",
    )
  ]),
  UnstableField("Kernel cache size(MB)", "cache_size", false, [
    Metadata(
      Datatype.doublePrecision,
      minValue: "0.0",
      maxValue: "inf",
      defaultValue: "200",
      description: "Kernel cache size(MB).",
    )
  ]),
  UnstableField("Class balance", "class_weight", false, [
    Metadata(
      Datatype.dictionary,
      description:
          "Class balance, i.e. using a dictionary and 2 classes: {0:1.2, 1:3.5}.",
    ),
    Metadata(
      Datatype.restrictedChoiceString,
      items: ["balanced"],
      description: "Class balance, can be set to 'balanced'.",
    )
  ]),
  UnstableField("Enable verbose output", "verbose", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description: "Whether to enable verbose output.",
    )
  ]),
  UnstableField(
      "Maximum number of training iterations", "max_iterations", false, [
    Metadata(
      Datatype.integer,
      minValue: "-1",
      maxValue: "inf",
      defaultValue: "-1",
      description:
          "Maximal number of training iterations, -1 designates no limit.",
    )
  ]),
  UnstableField("Decision function shape", "decision_function_shape", false, [
    Metadata(
      Datatype.restrictedChoiceString,
      defaultValue: "ovr",
      items: ["ovr", "ovo"],
      description: "Decision function shape.",
    )
  ]),
  UnstableField("Whether to break tie decision", "break_ties", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description:
          "Whether to break tie decision for ovr with more than 2 classes.",
    )
  ]),
];

const List<UnstableField> normalizationFields = [
  rootFolderFld,
  nbAttributesFld,
  UnstableField("Number of classes", "nb_classes", false, [
    Metadata(
      Datatype.integer,
      minValue: "2",
      maxValue: "inf",
      description: "Number of classes in the dataset.",
    )
  ]),
  UnstableField("Data files to normalize", "data_files", false, [
    Metadata(
      Datatype.listFilePath,
      description:
          "List of paths to data files to normalize, they are normalized with respect to the first one if normalization_file is not specified. Either 'data_files' or 'rule_files' must be specified.",
    )
  ]),
  UnstableField("Rule files to denormalize", "rule_files", false, [
    Metadata(
      Datatype.listFilePath,
      description:
          "List of paths to rule files to denormalize, denormalization is possible only if a normalization_file file or mus, sigmas and normalization_indices are given. Either 'data_files' or 'rule_files' must be specified.",
    )
  ]),
  UnstableField("String representing a missing value in the data",
      "missing_values", false, [
    Metadata(
      Datatype.string,
      description:
          "String representing a missing value in the data, put 'NaN' (or any string not present in the data) if there is no missing value, mandatory for normalization.",
    )
  ]),
  UnstableField("Normalization file", "normalization_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified.",
    )
  ]),
  UnstableField("Mus", "mus", false, [
    Metadata(
      Datatype.listDoublePrecision,
      minValue: "-inf",
      maxValue: "inf",
      description: "Mean or median of each attribute index of interest.",
    )
  ]),
  UnstableField("Sigmas", "sigmas", false, [
    Metadata(
      Datatype.listDoublePrecision,
      minValue: "-inf",
      maxValue: "inf",
      description: "Standard deviation of each attribute index of interest.",
    )
  ]),
  UnstableField("Normalization indices", "normalization_indices", false, [
    Metadata(
      Datatype.listInteger,
      minValue: "0",
      maxValue: "# attributes - 1",
      description:
          "Attribute indices to be normalized or denormalized, index starts at 0, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1]).",
    )
  ]),
  UnstableField(
      "Output normalization file", "output_normalization_file", false, [
    Metadata(
      Datatype.filePath,
      defaultValue: "normalization_stats.txt",
      description:
          "Path to the file where the mean and standard deviation of the normalized attributes will be stored if normalization_file is not specified.",
    )
  ]),
  UnstableField("Normalized files", "output_data_files", false, [
    Metadata(
      Datatype.listFilePath,
      description:
          "List of paths where the normalized files will be saved, it is mandatory to specify each of them if one is specified (default: <original_name>_normalized<original_extension>).",
    )
  ]),
  UnstableField("Normalized rule files", "output_rule_files", false, [
    Metadata(
      Datatype.listFilePath,
      description:
          "List of paths where the normalized rule files will be saved, it is mandatory to specify each of them if one is specified (default: <original_name>_denormalized<original_extension>).",
    )
  ]),
  UnstableField("Attributes file", "attributes_file", false, [
    Metadata(
      Datatype.filePath,
      description:
          "Path to the file containing the labels of attributes and classes. Mandatory if rules or normalization stats are written with attribute names.",
    )
  ]),
  UnstableField(
      "Whether to use median instead of mean normalize", "with_median", false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "false",
      description: "Whether to use median instead of mean to normalize.",
    )
  ]),
  UnstableField(
      "Whether to fill missing values with mean or median during normalization",
      "fill_missing_values",
      false, [
    Metadata(
      Datatype.boolean,
      defaultValue: "true",
      description:
          "Whether to fill missing values with mean (or median) during normalization.",
    )
  ]),
];
