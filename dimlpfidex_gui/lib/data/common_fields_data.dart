import 'package:dimlpfidex_gui/data/field.dart';

const rootFolderFld = Field(
  "Root folder",
  "root_folder",
  Datatype.directoryPath,
  description:
      "Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.",
);

const trainDataFileFld = Field(
  "Train data file",
  "train_data_file",
  Datatype.filePath,
  isRequired: true,
  description: "Path of file containing the train portion of the dataset.",
);

const trainPredFileFld = Field(
  "Train prediction file",
  "train_pred_file",
  Datatype.filePath,
  isRequired: true,
  description:
      "Path of file containing predictions on the train portion of the dataset.",
);

const nbAttributesFld = Field(
  "Number of attributes",
  "nb_attributes",
  Datatype.integer,
  isRequired: true,
  minValue: "1",
  maxValue: "inf",
  description: "Number of attributes in dataset.",
);
const nbClassesFld = Field(
  "Number of classes",
  "nb_classes",
  Datatype.integer,
  isRequired: true,
  minValue: "2",
  maxValue: "inf",
  description: "Number of classes in dataset.",
);

const nbQuantLevelsFld = Field(
  "Number of stairs in staircase function",
  "nb_quant_levels",
  Datatype.integer,
  minValue: "3",
  maxValue: "inf",
  defaultValue: "50",
  description: "Number of stairs in staircase activation function.",
);

const dropoutDimFld = Field(
  "Dimension dropout",
  "dropout_dim",
  Datatype.doublePrecision,
  minValue: "0.0",
  maxValue: "1.0",
  defaultValue: "0.0",
  description: "Dimension dropout parameter.",
);
const dropoutHypFld = Field(
  "Hyperplan dropout",
  "dropout_hyp",
  Datatype.doublePrecision,
  minValue: "0.0",
  maxValue: "1.0",
    defaultValue: "0.0",
  description: "Hyperplan dropout parameter.",
);

const rulesOutputFileFld = Field(
  "Rules output file",
  "rules_outfile",
  Datatype.filePath,
  description: "Path/name of rule(s) output file.",
);

const consoleFileFld = Field(
  "Console file",
  "console_file",
  Datatype.filePath,
  description:
      "Path of generated file that will contain the terminal output's redirection. If not specified, all output will be shown on your terminal.",
);

const musFld = Field(
  "Mus",
  "mus",
  Datatype.listDoublePrecision,
  minValue: "inf",
  maxValue: "inf",
  description:
      "Mean or median of each attribute index to denormalize in the rules.",
);

const sigmasFld = Field(
  "Sigmas",
  "sigmas",
  Datatype.listDoublePrecision,
  minValue: "inf",
  maxValue: "inf",
  description:
      "Standard deviation of each attribute index to denormalize in the rules.",
);

const normalizationIndicesFld = Field(
  "Normalization indices",
  "normalization_indices",
  Datatype.listInteger,
  minValue: "0",
  maxValue: "# attributes - 1",
  description:
      "Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1]).",
);

const seedFld = Field(
  "Seed",
  "seed",
  Datatype.integer,
  minValue: "0",
  maxValue: "inf",
  defaultValue: "0",
  description:
      "Seed for random number generation, 0=random. Anything else than 0 is a arbitrary seed that can be reused to obtain the same randomly generated sequence and therefore getting same results.",
);
