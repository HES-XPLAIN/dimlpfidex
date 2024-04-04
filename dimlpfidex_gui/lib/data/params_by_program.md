## Parameters list by program


### Dimlp
#### 1. DimlpCls
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--test_data_file <str>        Test data file
--weights_file <str>          Weights file
--nb_attributes <int [1,inf[> Number of input neurons
--nb_classes <int [2,inf[>    Number of output neurons
--hidden_layers_file <str>    Hidden layers file name

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--test_class_file <str>       Test true class file
--test_pred_outfile <str>     Output test prediction file name (default: dimlpTest.out)
--console_file <str>          File with console logs redirection
--stats_file <str>            Output file name with test accuracy
--hid_file <str>              Output file name with first hidden layer values (default: dimlpTest.hid)
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)

----------------------------

Execution example :

dimlp.dimlpCls("--test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --weights_file weights.wts --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --test_pred_outfile predTest.out --stats_file stats.txt --root_folder dimlp/datafiles")

---------------------------------------------------------------------
       
```
#### 2. DimlpTrn
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--train_data_file <str>       Train data file
--nb_attributes <int [1,inf[> Number of input neurons
--nb_classes <int [2,inf[>    Number of output neurons

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--attributes_file <str>       File of attributes
--valid_data_file <str>       Validation data file
--test_data_file <str>        Test data file
--weights_file <str>          Pretrained weights file
--train_class_file <str>      Train true class file
--test_class_file <str>       Test true class file
--valid_class_file <str>      Validation true class file
--weights_outfile <str>       Output weights file name (default: dimlp.wts)
--train_pred_outfile <str>    Output train prediction file name (default: dimlpTrain.out)
--test_pred_outfile <str>     Output test prediction file name (default: dimlpTest.out)
--valid_pred_outfile <str>    Output validation prediction file name (default: dimlpValidation.out)
--console_file <str>          File with console logs redirection
--stats_file <str>            Output file name with train, test and validation accuracy
--first_hidden_layer <int k*nb_attributes, k in [1,inf[>
                              Number of neurons in the first hidden layer (default: nb_attributes)
--hidden_layers <list<int [1,inf[>>
                              Number of neurons in each hidden layer, from the second layer through to the last
--hidden_layers_outfile <str> Output hidden layers file name (default: hidden_layers.out)
--with_rule_extraction <bool> Whether to extract rules with dimlp algorithm
--global_rules_outfile <str>  Rules output file
--learning_rate <float ]0,inf[>
                              Back-propagation learning parameter (default: 0.1)
--momentum <float [0,inf[>    Back-propagation momentum parameter (default: 0.6)
--flat <float [0,inf[>        Back-propagation flat spot elimination parameter (default: 0.01)
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)
--error_thresh <float [0,inf[>
                              Error threshold to stop training
--acc_thresh <float ]0,1]>    Accuracy threshold to stop training
--abs_error_thresh <float [0,inf[>
                              Absolute difference error threshold, 0 if not using this stopping criteria (default: 0)
--nb_epochs <int [1,inf[>     Number of epochs (default: 1500)
--nb_epochs_error <int [1,inf[>
                              Number of epochs to show error (default: 10)
--normalization_file <str>    File containing the mean and std of some attributes. Used to denormalize the rules if specified
--mus <list<float ]inf,inf[>> Mean or median of each attribute index to denormalize in the rules
--sigmas <list<float ]inf,inf[>>
                              Standard deviation of each attribute index to denormalize in the rules
--normalization_indices <list<int [0,nb_attributes-1]>>
                              Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])
--seed <int [0,inf[>          Seed, 0=random (default: 0)

----------------------------

Execution example :

dimlp.dimlpTrn("--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers 5 --nb_classes 2 --weights_outfile dimlpDatanormBT.wts --with_rule_extraction true --global_rules_outfile globalRules.rls --train_pred_outfile predTrain.out --test_pred_outfile predTest.out --stats_file stats.txt --root_folder dimlp/datafiles")

---------------------------------------------------------------------       
```
#### 3. DimlpPred
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--test_data_file <str>        Test data file
--weights_file <str>          Weights file
--nb_attributes <int [1,inf[> Number of input neurons
--nb_classes <int [2,inf[>    Number of output neurons
--hidden_layers_file <str>    Hidden layers file name

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--test_pred_outfile <str>     Output test prediction file name (default: dimlpTest.out)
--console_file <str>          File with console logs redirection
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)

----------------------------

Execution example :

dimlp.dimlpPred("--test_data_file datanormTest.txt --weights_file dimlpDatanorm.wts --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --test_pred_outfile predTest.out --root_folder dimlp/datafiles")

---------------------------------------------------------------------
```
#### 4. DimlpRul
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--train_data_file <str>       Train data file
--weights_file <str>          Weights file
--nb_attributes <int [1,inf[> Number of input neurons
--nb_classes <int [2,inf[>    Number of output neurons
--hidden_layers_file <str>    Hidden layers file name

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--attributes_file <str>       File of attributes
--valid_data_file <str>       Validation data file
--test_data_file <str>        Test data file
--train_class_file <str>      Train true class file
--test_class_file <str>       Test true class file
--valid_class_file <str>      Validation true class file
--global_rules_outfile <str>  Rules output file (default: dimlp.rls)
--console_file <str>          File with console logs redirection
--stats_file <str>            Output file name with train, test and validation accuracy
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)
--normalization_file <str>    File containing the mean and std of some attributes. Used to denormalize the rules if specified
--mus <list<float ]inf,inf[>> Mean or median of each attribute index to denormalize in the rules
--sigmas <list<float ]inf,inf[>>
                              Standard deviation of each attribute index to denormalize in the rules
--normalization_indices <list<int [0,nb_attributes-1]>>
                              Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])

----------------------------

Execution example :

dimlp.dimlpRul("--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --weights_file dimlpDatanorm.wts --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers_file hidden_layers.out --nb_classes 2 --global_rules_outfile globalRules.rls --stats_file stats.txt --root_folder dimlp/datafiles")

---------------------------------------------------------------------
```
#### 5. DimlpBT
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--train_data_file <str>       Train data file
--nb_attributes <int [1,inf[> Number of input neurons
--nb_classes <int [2,inf[>    Number of output neurons

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--nb_dimlp_nets <int [1,inf[> Number of networks (default: 25)
--attributes_file <str>       File of attributes
--test_data_file <str>        Test data file
--train_class_file <str>      Train true class file
--test_class_file <str>       Test true class file
--console_file <str>          File with console logs redirection
--weights_outfilename <str>   Output weights file name file (default: dimlpBT.wts
--train_pred_outfile <str>    Output train prediction file name (default: dimlpBTTrain.out)
--test_pred_outfile <str>     Output test prediction file name (default: dimlpBTTest.out)
--stats_file <str>            Output file name with train, test and validation accuracy and with the global train and test accuracy
--first_hidden_layer <int k*nb_attributes, k in [1,inf[>
                              Number of neurons in the first hidden layer (default: nb_attributes)
--hidden_layers <list<int [1,inf[>>
                              Number of neurons in each hidden layer, from the second layer through to the last
--hidden_layers_outfile <str> Output hidden layers file name (default: hidden_layers.out)
--with_rule_extraction <bool> Whether to extract rules with dimlpBT algorithm
--global_rules_outfile <str>  Rules output file
--momentum <float [0,inf[>    Back-propagation momentum parameter (default: 0.6)
--flat <float [0,inf[>        Back-propagation flat spot elimination parameter (default: 0.01)
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)
--error_thresh <float [0,inf[>
                              Error threshold to stop training
--acc_thresh <float ]0,1]>    Accuracy threshold to stop training
--abs_error_thresh <float [0,inf[>
                              Absolute difference error threshold, 0 if not using this stopping criteria (default: 0)
--nb_epochs <int [1,inf[>     Number of epochs (default: 1500)
--nb_epochs_error <int [1,inf[>
                              Number of epochs to show error (default: 10)
--nb_ex_per_net <int [1,inf[> Number of examples for one single network, 0 for all examples, it is not recommended to change this value (default: 0)
--normalization_file <str>    File containing the mean and std of some attributes. Used to denormalize the rules if specified
--mus <list<float ]inf,inf[>> Mean or median of each attribute index to denormalize in the rules
--sigmas <list<float ]inf,inf[>>
                              Standard deviation of each attribute index to denormalize in the rules
--normalization_indices <list<int [0,nb_attributes-1]>>
                              Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])
--seed <int [0,inf[>          Seed, 0=random (default: 0)

----------------------------

Execution example :

dimlp.dimlpBT("--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --nb_attributes 16 --hidden_layers 5 --nb_classes 2 --nb_dimlp_nets 2 --weights_outfile dimlpDatanormBT.wts --with_rule_extraction true --global_rules_outfile globalRules.rls --train_pred_outfile predTrain.out --test_pred_outfile predTest.out --stats_file stats.txt --root_folder dimlp/datafiles")

---------------------------------------------------------------------
```

---

### Fidex
#### 1. Fidex
```
 ---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--train_data_file <str>       Train data file
--train_pred_file <str>       Train prediction file
--train_class_file <str>      Train true class file, not mandatory if classes are specified in train data file
--test_data_file <str>        Test sample(s) data file with data, prediction(if no --test_pred_file) and true class(if no --test_class_file)
--weights_file <str>          Weights file (not mandatory if a rules file is given with --rules_file)
--rules_file <str>            Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)
--rules_outfile <str>         Rule(s) output file
--nb_attributes <int [1,inf[> Number of attributes in dataset
--nb_classes <int [2,inf[>    Number of classes in dataset

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--test_pred_file <str>        Test prediction file
--test_class_file <str>       Test true class file. If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case)
--attributes_file <str>       File of attributes
--stats_file <str>            Output statistic file name
--console_file <str>          File with console logs redirection
--max_iterations <int [1,inf[>
                              Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images (default: 10)
--min_covering <int [1,inf[>  Minimum covering number (default: 2)
--covering_strategy <bool>    Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)
--max_failed_attempts <int [0,inf[>
                              Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)
--min_fidelity <float [0,1]>  Minimal rule fidelity accepted when generating a rule (default: 1.0)
--lowest_min_fidelity <float [0,1]>
                              Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)
--dropout_dim <float [0,1]>   Dimension dropout parameter (default: 0.0)
--dropout_hyp <float [0,1]>   Hyperplan dropout parameter (default: 0.0)
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)
--decision_threshold <float [0,1]>
                              Decision threshold for predictions, you need to specify the index of positive class if you want to use it
--positive_class_index <int [0,nb_classes-1]>
                              Index of positive class for the usage of decision threshold, index starts at 0
--normalization_file <str>    File containing the mean and std of some attributes. Used to denormalize the rules if specified
--mus <list<float ]inf,inf[>> Mean or median of each attribute index to denormalize in the rules
--sigmas <list<float ]inf,inf[>>
                              Standard deviation of each attribute index to denormalize in the rules
--normalization_indices <list<int [0,nb_attributes-1]>>
                              Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])
--seed <int [0,inf[>          Seed, 0=random (default: 0)

----------------------------

Execution example :

fidex.fidex("--train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --test_data_file testSampleDataCombine.txt --nb_attributes 16 --nb_classes 2 --weights_file weights.wts --rules_outfile rules.rls --stats_file stats.txt --root_folder dimlp/datafiles")

---------------------------------------------------------------------
```
#### 2. FidexGlo
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--test_data_file <str>        Test sample(s) data file with data and prediction(if no --test_pred_file), classes may been added here if launching with fidex(--with_fidex)
--global_rules_file <str>     Ruleset input file
--nb_attributes <int [1,inf[> Number of attributes in dataset
--nb_classes <int [2,inf[>    Number of classes in dataset

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--attributes_file <str>       File of attributes> Mandatory if rules file contains attribute names, if not, do not add it
--test_pred_file <str>        Test prediction file> if given, --test_data_file needs to have only test datas
--explanation_file <str>      Explanation(s) output file name
--console_file <str>          File with console logs redirection
--decision_threshold <float [0,1]>
                              Decision threshold for predictions, you need to specify the index of positive class if you want to use it
--positive_class_index <int [0,nb_classes-1]>
                              Index of positive class for the usage of decision threshold, index starts at 0
--with_fidex <bool>           Whether to use Fidex if no rule is found in global rules (default: False)
--with_minimal_version <bool> Whether to use minimal version, which only gets correct activated rules and if with_fidex, launches Fidex when no such rule is found (default: False)

----------------------------

If using fidex :

Required :

--train_data_file <str>       Train data file
--train_pred_file <str>       Train prediction file
--train_class_file <str>      Train true class file, not mandatory if classes are specified in train data file
--weights_file <str>          Weights file (not mandatory if a rules file is given with --rules_file)
--rules_file <str>            Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)

Optional :

--test_class_file <str>       Test true class file, classes can be specified in test data file
--max_iterations <int [1,inf[>
                              Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images (default: 10)
--min_covering <int [1,inf[>  Minimum covering number (default: 2)
--covering_strategy <bool>    Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)
--max_failed_attempts <int [0,inf[>
                              Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)
--min_fidelity <float [0,1]>  Minimal rule fidelity accepted when generating a rule (default: 1.0)
--lowest_min_fidelity <float [0,1]>
                              Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)
--dropout_dim <float [0,1]>   Dimension dropout parameter (default: 0.0)
--dropout_hyp <float [0,1]>   Hyperplan dropout parameter (default: 0.0)
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)
--normalization_file <str>    File containing the mean and std of some attributes. Used to denormalize the rules if specified
--mus <list<float ]inf,inf[>> Mean or median of each attribute index to denormalize in the rules
--sigmas <list<float ]inf,inf[>>
                              Standard deviation of each attribute index to denormalize in the rules
--normalization_indices <list<int [0,nb_attributes-1]>>
                              Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])
--seed <int [0,inf[>          Seed, 0=random (default: 0)

----------------------------

Execution example :

fidex.fidexGlo("--test_data_file datanormTest.txt --test_pred_file predTest.out --global_rules_file globalRules.rls --nb_attributes 16 --nb_classes 2 --explanation_file explanation.txt --root_folder dimlp/datafiles --with_fidex true --train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --test_class_file dataclass2Test.txt --weights_file weights.wts")

---------------------------------------------------------------------
```
#### 3. FidexGloRules
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--train_data_file <str>       Train data file
--train_pred_file <str>       Train prediction file
--train_class_file <str>      Train true class file, not mandatory if classes are specified in train data file
--weights_file <str>          Weights file (not mandatory if a rules file is given with --rules_file)
--rules_file <str>            Rules file to be converted to hyperlocus (not mandatory if a weights file is given with --weights_file)
--global_rules_outfile <str>  Rules output file>
--heuristic <int [1,3]>       Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo
--nb_attributes <int [1,inf[> Number of attributes in dataset
--nb_classes <int [2,inf[>    Number of classes in dataset

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--attributes_file <str>       File of attributes
--console_file <str>          File with console logs redirection
--max_iterations <int [1,inf[>
                              Max iteration number, also the max possible number of attributs in a rule, should be 25 if working with images (default: 10)
--min_covering <int [1,inf[>  Minimum covering number (default: 2)
--covering_strategy <bool>    Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (default: True)
--max_failed_attempts <int [0,inf[>
                              Maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (default: 30)
--min_fidelity <float [0,1]>  Minimal rule fidelity accepted when generating a rule (default: 1.0)
--lowest_min_fidelity <float [0,1]>
                              Minimal min_fidelity to which we agree to go down during covering_strategy (default: 0.75)
--dropout_dim <float [0,1]>   Dimension dropout parameter (default: 0.0)
--dropout_hyp <float [0,1]>   Hyperplan dropout parameter (default: 0.0)
--nb_quant_levels <int [3,inf[>
                              Number of stairs in staircase activation function (default: 50)
--decision_threshold <float [0,1]>
                              Decision threshold for predictions, you need to specify the index of positive class if you want to use it
--positive_class_index <int [0,nb_classes-1]>
                              Index of positive class for the usage of decision threshold, index starts at 0
--normalization_file <str>    File containing the mean and std of some attributes. Used to denormalize the rules if specified
--mus <list<float ]inf,inf[>> Mean or median of each attribute index to denormalize in the rules
--sigmas <list<float ]inf,inf[>>
                              Standard deviation of each attribute index to denormalize in the rules
--normalization_indices <list<int [0,nb_attributes-1]>>
                              Attribute indices to denormalize in the rules, only used when no normalization_file is given, index starts at 0 (default: [0,...,nb_attributes-1])
--nb_threads <int [1,nb_cores]>
                              Number of threads used for computing the algorithm, 1=sequential execution (default: 1)
--seed <int [0,inf[>          Seed, 0=random (default: 0)

----------------------------

Execution example :

fidex.fidexGloRules("--train_data_file datanormTrain.txt --train_pred_file predTrain.out --train_class_file dataclass2Train.txt --weights_file weights.wts --nb_attributes 16 --nb_classes 2 --heuristic 1 --global_rules_outfile globalRules.rls --root_folder dimlp/datafiles")

---------------------------------------------------------------------
```
#### 4. FidexGloStats
```
---------------------------------------------------------------------

Warning! The files are localised with respect to root folder dimlpfidex.
The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.

----------------------------

Required parameters:

--test_data_file <str>        Test data file
--test_pred_file <str>        Test prediction file
--test_class_file <str>       Test true class file, not mandatory if classes are specified in test data file
--global_rules_file <str>     Ruleset input file
--nb_attributes <int [1,inf[> Number of attributes in dataset
--nb_classes <int [2,inf[>    Number of classes in dataset

----------------------------

Optional parameters:

--json_config_file <str>      JSON file to configure all parameters. If used, this must be the sole argument and must specify the file's relative path
--root_folder <str>           Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder
--attributes_file <str>       File of attributes> Mandatory if rules file contains attribute names, if not, do not add it
--stats_file <str>            Output statistic file name
--global_rules_outfile <str>  Global ruleset output file with stats on test set, if you want to compute statistics of global rules on tests set
--console_file <str>          File with console logs redirection
--decision_threshold <float [0,1]>
                              Decision threshold for predictions, use if it was used in FidexGloRules, you need to specify the index of positive class if you want to use it
--positive_class_index <int [0,nb_classes-1]>
                              Index of positive class for the usage of decision threshold and to compute true/false positive/negative rates, index starts at 0

----------------------------

Execution example :

fidex.fidexGloStats("--test_data_file datanormTest.txt --test_pred_file predTest.out --test_class_file dataclass2Test.txt --global_rules_file globalRules.rls --nb_attributes 16 --nb_classes 2 --stats_file stats.txt --root_folder dimlp/datafiles")

---------------------------------------------------------------------
```