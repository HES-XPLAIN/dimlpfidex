## Parameters list by program


### Dimlp
#### 1. DimlpCls
```
-------------------------------------------------
       

  dimlpCls --test_data_file <test set file (path with respect to specified root folder)> 
  
  --weights_file <file of weights> 
  
  --nb_attributes <number of input neurons> --nb_classes <number of output neurons>
  
   <Options>
       

  Options are: 
       
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  
  --test_class_file <file of classes> 
  
  --test_pred_outfile <output prediction file (dimlp.out by default)> 
  
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  
  --stats_file <output file with test accuracy> 
  
  --hid_file <output file with first hidden layer values (dimlp.hid by default)> 
  
  --h1 <number of neurons in the first hidden layer> 
  (if not specified this number will be equal to the 
  number of input neurons) 
  
  --hk <number of neurons in the kth hidden layer> 
  
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 

  -------------------------------------------------
       
```
#### 2. DimlpTrn
```
-------------------------------------------------
       

  dimlpTrn --train_data_file <training set file(path with respect to specified root folder)> 
  --nb_attributes <number of input neurons> --nb_classes <number of output neurons>
   <Options>
       

  Options are: 
       
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --attributes_file <file of attributes> 
  --valid_data_file <validation set file> 
  --test_data_file <testing set file> 
  --weights_file <file of pretrained weights> 
  --train_class_file <file of train classes> 
  --test_class_file <file of test classes> 
  --valid_class_file <file of validation classes> 
  --weights_outfile <output weight file (dimlp.wts by default)> 
  --train_pred_outfile <output train prediction file (dimlpTrain.out by default)> 
  --test_pred_outfile <output test prediction file (dimlpTest.out by default)> 
  --valid_pred_outfile <output validation prediction file (dimlpValidation.out by default)> 
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  --stats_file <output file with train, test and validation accuracy> 
  --H1 <number of neurons in the first hidden layer> 
  (if not specified this number will be equal to the 
  number of input neurons) 
  --Hk <number of neurons in the kth hidden layer> 
  --with_rule_extraction (RULE EXTRACTION) 
  --global_rules_outfile <extraction ruleFile>  // If we want to extract rules in a rulesFile instead of console
  --learning_rate <back-propagation learning parameter (Eta, 0.1 by default)> 
  --momentum <back-propagation momentum parameter (Mu, 0.6 by default)> 
  --flat <back-propagation flat spot elimination parameter (Flat, 0.01 by default)> 
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 
  --error_thresh <error threshold (None by default )> 
  --acc_thresh <accuracy threshold (None by default)> 
  --abs_error_thresh <absolute difference error threshold (0 by default)> 
  --nb_epochs <number of epochs (1500 by default)> 
  --nb_epochs_error <number of epochs to show error (10 by default)> 
  --normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified> 
  --mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules> 
  --sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules> 
  --normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)> 
  --seed <seed (0=random, default)>

  -------------------------------------------------
       
```
#### 3. DimlpPred
```
-------------------------------------------------
            

  dimlpPred --test_data_file <test set file (path with respect to specified root folder)> 
  --weights_file <file of weights> 
  --nb_attributes <number of input neurons> --nb_classes <number of output neurons>

   <Options>
            

  Options are: 
            
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --test_pred_outfile <output prediction file (dimlpTest.out by default)>
  --console_file <file where you redirect console result> // If we want to redirect console result to file
  --H1 <number of neurons in the first hidden layer> 
  (if not specified this number will be equal to the 
  number of input neurons) 
  --Hk <number of neurons in the kth hidden layer> 
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 

  -------------------------------------------------
            
```
#### 4. DimlpRul
```
-------------------------------------------------
       

  dimlpRul --train_data_file <training set file(path with respect to specified root folder)> 
  --weights_file <file of weights> 
  --nb_attributes <number of input neurons> --nb_classes <number of output neurons>
   <Options>
       

  Options are: 
       
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --attributes_file <file of attributes> 
  --valid_data_file <validation set file> 
  --test_data_file <testing set file> 
  --train_class_file <file of train classes> 
  --test_class_file <file of test classes> 
  --valid_class_file <file of validation classes> 
  --global_rules_outfile <rulesFile (dimlp.rls by default)> 
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  --stats_file <output file with train, test and validation accuracy> 
  --H1 <number of neurons in the first hidden layer> 
  (if not specified this number will be equal to the 
  number of input neurons) 
  --Hk <number of neurons in the kth hidden layer> 
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 
  --normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified> 
  --mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules> 
  --sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules> 
  --normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)> 

  -------------------------------------------------
       
```
#### 5. DimlpBT
```
-------------------------------------------------
       

  dimlpBT --train_data_file <training set file(path with respect to specified root folder)> 
  --nb_attributes <number of input neurons> --nb_classes <number of output neurons>

   <Options>
       

  Options are: 
       
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --nb_dimlp_nets <number of networks (25 by default)> 
  --attributes_file <file of attributes> 
  --test_data_file <testing set file> 
  --train_class_file <file of train classes> 
  --test_class_file <file of test classes> 
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  --weights_generic_outfilename <output weights generic name file(without .wts, dimlpBT by default)> 
  --train_pred_outfile <output train prediction file (dimlpBTTrain.out by default)> 
  --test_pred_outfile <output test prediction file (dimlpBTTest.out by default)> 
  --stats_file <output file with train, test and validation accuracy and with the global accuracy for train and test> 
  --H1 <number of neurons in the first hidden layer> 
  (if not specified this number will be equal to the 
  number of input neurons) 
  --Hk <number of neurons in the kth hidden layer> 
  --with_rule_extraction (RULE EXTRACTION) 
  --global_rules_outfile <extraction ruleFile>  // If we want to extract rules in a rulesFile instead of console
  --learning_rate <back-propagation learning parameter (Eta, 0.1 by default)> 
  --momentum <back-propagation momentum parameter (Mu, 0.6 by default)> 
  --flat <back-propagation flat spot elimination parameter (Flat, 0.01 by default)> 
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 
  --error_thresh <error threshold (None by default)> 
  --acc_thresh <accuracy threshold (None by default)> 
  --abs_error_thresh <absolute difference error threshold> 
  --nb_epochs <number of epochs (0 by default)> 
  --nb_epochs_error <number of epochs to show error (1500 by default)> 
  --nb_ex_per_net <number of examples for one single network (all examples by default (value 0, it is recommended not to change this value))> 
  --normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified> 
  --mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules> 
  --sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules> 
  --normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)> 
  --seed <seed (0=random, default)>

  -------------------------------------------------
       
```

---

### Fidex
#### 1. Fidex
```
 -------------------------------------------------
            

  Obligatory parameters : 
            
  fidex --train_data_file <train data file> 
  --train_pred_file <train prediction file> 
  --train_class_file <train true class file, not mendatory if classes are specified in train data file> 
  --test_data_file <test sample(s) data file with data, prediction(if no --test_pred_file) and true class(if no --test_class_file)> 
  --weights_file <weights file. In case of bagging, put prefix of files, ex: dimlpBT, files need to be in the form dimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with --rules_file] 
  --rules_file <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] 
  --rules_outfile <Rule(s) output file> 
  --nb_attributes <number of attributes> 
  --nb_classes <number of classes> 
  <Options>
            

  Options are: 
            
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)> 
  --test_pred_file <test prediction file> 
  --test_class_file <test true class file> If at least --test_pred_file is specified, --test_data_file needs to have only test datas and eventually classes on same line (don't add --test_class_file in this case) 
  --attributes_file <file of attributes> 
  --stats_file <output statistic file> 
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  --max_iterations <max iteration number, also the max possible number of attributs in a rule (10 by default, should put 25 if working with images)> 
  --min_covering <minimum covering number (2 by default)> 
  --covering_strategy <if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (True by default)> 
  --max_failed_attempts <maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (30 by default)> 
  --min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)> 
  --lowest_min_fidelity <minimal min_fidelity to which we agree to go down during covering_strategy (0.75 by default)> 
  --dropout_dim <dimension dropout parameter (None by default)> 
  --dropout_hyp <hyperplan dropout parameter (None by default)> 
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 
  --decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)> 
  --positive_class_index <index of positive class for the usage of decision threshold (None by default, 0 for first one)> 
  --normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified> 
  --mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules> 
  --sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules> 
  --normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)> 
  --seed <seed (0=random, default)>

  -------------------------------------------------
            
```
#### 2. FidexGlo
```
-------------------------------------------------
            

  Obligatory parameters : 
            

  fidexGlo --test_data_file <test sample(s) data file with data and prediction(if no --test_pred_file), classes may been added here if launching with fidex(--with_fidex)> 
  --global_rules_file <ruleset input file> 
  --nb_attributes <number of attributes> 
  --nb_classes <number of classes> 
  <Options>
            

  Options are: 
            
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --attributes_file <file of attributes> Mandatory if rules file contains attribute names, if not, do not add it 
  --test_pred_file <test prediction file> if given, --test_data_file needs to have only test datas 
  --explanation_file <Explanation(s) output file> 
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  --decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)> 
  --positive_class_index <index of positive class for the usage of decision threshold (None by default, 0 for first one)> 
  --with_fidex <with Fidex if not rule is given (False by default)> 
  --with_minimal_version <minimal version, only correct activated rules, launch Fidex when no such rule is found(if with fidex) (False by default)> 
            
  If using fidex : 
  Obligatory : 
  --train_data_file <train data file> 
  --train_pred_file <train prediction file> 
  --train_class_file <train true class file, not mendatory if classes are specified in train data file> 
  --weights_file <weights file. In case of bagging, put prefix of files, ex: dimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with --rules_file]  
  --rules_file <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given]  
  Options : 
  --test_class_file <test true class file, classes can be specified in test data file> 
  --nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)> 
  --max_iterations <max iteration number, also the max possible number of attributs in a rule (10 by default, should put 25 if working with images)> 
  --min_covering <minimum covering number (2 by default)> 
  --covering_strategy <if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (True by default)> 
  --max_failed_attempts <maximum number of failed attempts to find Fidex rule when covering is 1 and covering strategy is used (30 by default)> 
  --min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)> 
  --lowest_min_fidelity <minimal min_fidelity to which we agree to go down during covering_strategy (0.75 by default)> 
  --dropout_dim <dimension dropout parameter (None by default)> 
  --dropout_hyp <hyperplan dropout parameter (None by default)> 
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 
  --normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified> 
  --mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules> 
  --sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules> 
  --normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)> 
  --seed <seed (0=random, default)>

  -------------------------------------------------
            
```
#### 3. FidexGloRules
```
 -------------------------------------------------
       

  Obligatory parameters : 
       
  fidexGloRules --train_data_file <train data file> --train_pred_file <train prediction file> --train_class_file <train true class file, not mendatory if classes are specified in train data file> 
  --weights_file <weights file. In case of bagging, put prefix of files, ex: dimlpBT, files need to be in the form dimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with --rules_file] 
  --rules_file <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given] 
  --global_rules_outfile <Rules output file> 
  --heuristic <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo> 
  --nb_attributes <number of attributes> 
  --nb_classes <number of classes> 
  <Options>
       

  Options are: 
       
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)> 
  --attributes_file <file of attributes> 
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  --max_itertions <max iteration number, also the max possible number of attributs in a rule (10 by default, should put 25 if working with images)> 
  --min_covering <minimum covering number (2 by default)> 
  --dropout_dim <dimension dropout parameter> 
  --dropout_hyp <hyperplan dropout parameter> 
  --max_failed_attempts <maximum number of failed attempts to find Fidex rule when covering is 1 (30 by default)> 
  --nb_quant_levels <number of stairs in staircase activation function (50 by default)> 
  --decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)> 
  --positive_class_index <index of positive class for the usage of decision threshold (None by default, 0 for first one)> 
  --normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified> 
  --mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules> 
  --sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules> 
  --normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)> 
  --nb_threads <number of threads used for computing the algorithm (default=1, this means by default its a sequential execution)> 
  --covering_strategy <if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (True by default)> 
  --min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)> 
  --lowest_min_fidelity <minimal min_fidelity to which we agree to go down when computing a rule (0.75 by default)> 
  --seed <seed (0=random, default)>

  -------------------------------------------------
       
```
#### 4. FidexGloStats
```
-------------------------------------------------
            

  Obligatory parameters : 
            
  fidexGloStats --test_data_file <test data file> --test_pred_file <test prediction file> --test_class_file <test true class file, not mendatory if classes are specified in test data file> 
  --global_rules_file <rules input file> 
  --nb_attributes <number of attributes> 
  --nb_classes <number of classes> 
  <Options>
            

  Options are: 
            
  --root_folder <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder> 
  --attributes_file <file of attributes> Mandatory if rules file contains attribute names, if not, do not add it 
  --stats_file <stats output file> 
  --global_rules_outfile <global rules output file with stats on test set> If you want to compute statistics of global rules on tests set 
  --console_file <file where you redirect console result>  // If we want to redirect console result to file
  --decision_threshold <decision threshold for predictions, use if it was used in FidexGlo, need to specify the index of positive class if you want to use it (None by default)> 
  --positive_class_index <index of positive class sample to compute true/false positive/negative rates (None by default, put 0 for first class)>  // If we want to compute TP, FP, TN, FN

  -------------------------------------------------
            
```
