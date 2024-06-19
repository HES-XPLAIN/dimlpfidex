#!/bin/bash

# This file is used to test fidex algo
# EXE="bin/fidexGloRules"
# BASE_DIR="$HOME/Documents/hepia"
# TESTS_DIR="$BASE_DIR/dimlpfidex-tests/"
# CURRENT_VERSION_DIR="$BASE_DIR/dimlpfidex/"
# DATASET_DIR="data/pre-act/"
# DATA_DIR="${DATASET_DIR}data/"
# OUT_DIR="${DATA_DIR}out/"

EXE="bin/fidexGloRules"
BASE_DIR="$HOME/Documents/hepia/dimlpfidex-json"
CURRENT_VERSION_DIR="$BASE_DIR/dimlpfidex/"
TESTS_DIR="${CURRENT_VERSION_DIR}fidex/cpp/tests/"
MAIN_VERSION_DIR="${BASE_DIR}dimlpfidex-main/dimlpfidex/"
DATA_DIR="${TESTS_DIR}dataset/data/"
OUT_DIR="${TESTS_DIR}dataset/out/"

to_test_dir=$CURRENT_VERSION_DIR

# utils
RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

# default & optional args
# root_directory="${full_dir}"
# nb_dimlp_nets=1
# attributes_file="${DATA_DIR}attributes.txt"
# console_file=""
# max_iterations=50
# min_covering=2
# dropout_dim=0.7
# dropout_hyp=0.7
# max_failed_attempts=10
# nb_stairs=50
# decision_threshold=-1.0
# index_positive_class=-1
# nb_threads_used=4
# min_fidelity=1
# seed=0
# nb_attributes=31
# nb_classes=7

# to choose which version to test
# to_test_dir=$CURRENT_VERSION_DIR
# full_dir="${to_test_dir}${DATA_DIR}${OUT_DIR}"

# nb_attributes=84
# nb_classes=2
nb_attributes=31
nb_classes=7
nb_dimlp_nets=1
console_file=""
heuristic=1
max_iterations=50
min_covering=2
dropout_dim=0.7
dropout_hyp=0.7
max_failed_attempts=10
nb_stairs=50
decision_threshold=-1.0
index_positive_class=-1
nb_threads_used=4
min_fidelity=1
seed=0

date_str=$(date "+%d%m%y_%H%M%S")

# root_directory="${full_dir}"
train_data_file="${DATA_DIR}train.txt"
train_pred_file="${DATA_DIR}train.out"
train_true_classes="${DATA_DIR}train_true_classes.txt"
weigths_file="${DATA_DIR}weights.wts"
attributes_file="${DATA_DIR}attributes.txt"
# input_rules_file=""
output_rules_file="${OUT_DIR}tests_fidexglo_${date_str}.rls"

# train_data_file="${DATA_DIR}obesity_train.txt"
# train_pred_file="${DATA_DIR}obesity_train.out"
# train_true_classes="${DATA_DIR}obesity_train_true_classes.txt"
# weigths_file="${DATA_DIR}obesity.wts"
# input_rules_file=""
# output_rules_file="${OUT_DIR}tests_fidexglo_${date_str}.rls"
# heuristic=1
# nb_attributes=31
# nb_classes=7



# redirection type, uncomment what fits your needs
REDIRECTION="/dev/null"                 # silent
# REDIRECTION="/dev/stdout"               # in console
# REDIRECTION="${OUT_DIR}output.txt"      # in file

echo "Tests ran on $(date "+%d/%m/%y %R")"
echo "$date_str" >> $REDIRECTION

SUCCESS=0
TOTAL=0

# utils section
assert_success()
{
    test_name=$1
    code=$2
    TOTAL=$((TOTAL+1))

    if [ $code -eq 0 ] ; then
        SUCCESS=$((SUCCESS+1))
        echo -e "${GREEN}[${test_name}] Test passed !${RESET}"
    else
        echo -e "${RED}[${test_name}] Test failed...${RESET}"
    fi
}

assert_failure()
{
    test_name=$1
    code=$2
    TOTAL=$((TOTAL + 1))
    if [ $code -eq 0 ] ; then
        echo -e "${RED}[${test_name}] Test failed...${RESET}"
    else
        SUCCESS=$((SUCCESS+1))
        echo -e "${GREEN}[${test_name}] Test passed !${RESET}"
    fi
}

print_bold()
{
    str="$1"
    bold=$(tput bold)
    reset=$(tput sgr0)
    echo "${bold}${str}${reset}"
}

random_float()
{
    min=$1
    max=$2
    seq "$min" .01 "$max" | shuf | head -n1
}

random_int()
{
    min=$1
    max=$2
    shuf -i "$min-$max" -n 1
}





# tests section
test_help_output()
{
    test_name="Help output"
    print_bold "$test_name"
    # eval $(${to_test_dir}${EXE} ${REDIRECTION})
    "${to_test_dir}${EXE}" >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_train_data_file()
{
    test_name="Train data file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Train data file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "123" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_train_predictions_file()
{
    test_name="Train predictions file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Train predictions file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "123" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_true_classes_file()
{
    test_name="Train true classes file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Train true classes file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "123" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_weights_file()
{
    test_name="Weights file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Weights file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "123" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_output_rules_file()
{
    test_name="Output rules file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_heuristic_arg()
{
    test_name="Heuristic argument missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Heuristic argument datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "abc" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_attributes_nb_arg()
{
    test_name="Attributes number argument missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Attributes number argument datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "abc" \
    --nb_classes "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_classes_nb_arg()
{
    test_name="Classes number argument missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Classes number argument datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "abc" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

# test_input_rules_file()
# {
    # print_bold "Testing -f missing..."
    # "${to_test_dir}${EXE}" \
    # --train_data_file "${train_data_file}" \
    # --train_pred_file "${train_pred_file}" \
    # --train_class_file "${train_true_classes}" \
    # --weights_file "${weigths_file}" \
    # --global_rules_outfile "${output_rules_file}" \
    # --heuristic "${heuristic}" \
    # --nb_attributes "${nb_attributes}" \
    # --nb_classes "${nb_classes}" \
    #
    # code=$?
    # assert_failure "-f missing" $code
# }

test_heuristics()
{
    print_bold "Different heuristics"
    dd=0.7
    dh=0.7
    nth=6

    for heuristic_val in 1 2 3
    do
        "${to_test_dir}${EXE}" \
        --train_data_file "${train_data_file}" \
        --train_pred_file "${train_pred_file}" \
        --train_class_file "${train_true_classes}" \
        --weights_file "${weigths_file}" \
        --global_rules_outfile "${output_rules_file}" \
        --attributes_file "${attributes_file}" \
        --heuristic "${heuristic_val}" \
        --dropout_dim "${dd}" \
        --dropout_hyp "${dh}" \
        --nb_attributes "${nb_attributes}" \
        --nb_classes "${nb_classes}" \
        --max_iterations "${max_iterations}" \
        --min_fidelity "${min_fidelity}" \
        --seed "${seed}" \
        --nb_threads "${nth}" \
        --max_failed_attempts "${max_failed_attempts}" \
        >>"$REDIRECTION"
        code=$?
        assert_success "Heuristic ${heuristic_val}" $code
    done
}

test_dropouts()
{
    nth=6
    print_bold "Different dropouts"
    for dd in 0.5 0.75 0.9
    do
        for dh in 0.5 0.75 0.9
        do
            "${to_test_dir}${EXE}" \
            --train_data_file "${train_data_file}" \
            --train_pred_file "${train_pred_file}" \
            --train_class_file "${train_true_classes}" \
            --weights_file "${weigths_file}" \
            --global_rules_outfile "${output_rules_file}" \
            --attributes_file "${attributes_file}" \
            --heuristic "${heuristic}" \
            --nb_attributes "${nb_attributes}" \
            --nb_classes "${nb_classes}" \
            --dropout_dim "${dd}" \
            --nb_threads ${nth} \
            --max_iterations "${max_iterations}" \
            --min_fidelity "${min_fidelity}" \
            --dropout_hyp "${dh}" \
            >>"$REDIRECTION"
            code=$?
            assert_success "Dropout dim: ${dd}, dropout hyp: ${dh}" $code
        done
    done
}

test_random_seed()
{
    print_bold "Random seed"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    --seed 0 \
    >>"$REDIRECTION"

    code=$?
    assert_success "Random seed" $code

    fixed_seed=10
    print_bold "Fixed seed (${fixed_seed})"
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    --seed "$fixed_seed" \
    >>"$REDIRECTION"

    code=$?
    assert_success "Fixed seed" $code
}

test_nb_threads()
{
    print_bold "Different threads"
    for threads in 2 4 8
    do
        "${to_test_dir}${EXE}" \
        --train_data_file "${train_data_file}" \
        --train_pred_file "${train_pred_file}" \
        --train_class_file "${train_true_classes}" \
        --weights_file "${weigths_file}" \
        --global_rules_outfile "${output_rules_file}" \
        --attributes_file "${attributes_file}" \
        --heuristic "${heuristic}" \
        --dropout_dim "${dd}" \
        --dropout_hyp "${dh}" \
        --nb_attributes "${nb_attributes}" \
        --nb_classes "${nb_classes}" \
        --max_iterations "${max_iterations}" \
        --min_fidelity "${min_fidelity}" \
        --seed "${seed}" \
        --nb_threads "${threads}" \
        --max_failed_attempts "${max_failed_attempts}" \
        >>"$REDIRECTION"
        code=$?
        assert_success "With ${threads} threads" $code
    done
}

random_test_generation()
{
    idx=$1
    print_bold "Randomly generated test #${idx}"
    nb_dimlp_nets=1
    max_iterations=$(random_int 1 75)
    min_covering=$(random_int 2 10)
    dd=$(random_float 0.4 0.95)
    dh=$(random_float 0.4 0.95)
    max_failed_attempts=$(random_int 1 15)
    nb_stairs=50
    heuristic=$(random_int 1 3)
    nb_threads_used=$(random_int 4 8)
    min_fidelity=$(random_float 0.5 1.0)
    seed=$(random_int 1 10000)
    # decision_threshold=-1.0
    # index_positive_class=-1

    # missing -r, -S, -f, -x & -t
    "${to_test_dir}${EXE}" \
    --train_data_file "${train_data_file}" \
    --train_pred_file "${train_pred_file}" \
    --train_class_file "${train_true_classes}" \
    --weights_file "${weigths_file}" \
    --global_rules_outfile "${output_rules_file}" \
    --heuristic "${heuristic}" \
    --nb_attributes "${nb_attributes}" \
    --nb_classes "${nb_classes}" \
    --nb_dimlp_nets "${nb_dimlp_nets}" \
    --attributes_file "${attributes_file}" \
    --max_iterations "${max_iterations}" \
    --min_covering "${min_covering}" \
    --dropout_dim "${dd}" \
    --dropout_hyp "${dh}" \
    --max_failed_attempts "${max_failed_attempts}" \
    --nb_quant_levels "${nb_stairs}" \
    --nb_threads "${nb_threads_used}" \
    --min_fidelity "${min_fidelity}" \
    --seed "${seed}" \
    >>"$REDIRECTION"

    code=$?
    assert_success "Randomly generated test #${idx}" $code
}

# used to check if heuristics produces the same output if same seed is used
test_same_seed_same_output()
{
    h=$1
    sd=$(random_int 1 1000)
    print_bold "Same seed, same output..."
    for i in {1..2}
    do
        print_bold "Intermediate Heuristic #${h} execution..."
        "${to_test_dir}${EXE}" \
        --train_data_file "${train_data_file}" \
        --train_pred_file "${train_pred_file}" \
        --train_class_file "${train_true_classes}" \
        --weights_file "${weigths_file}" \
        --global_rules_outfile "${TESTS_DIR}test_same_seed_${i}.rls" \
        --heuristic "${h}" \
        --nb_attributes "${nb_attributes}" \
        --nb_classes "${nb_classes}" \
        --nb_dimlp_nets "${nb_dimlp_nets}" \
        --attributes_file "${attributes_file}" \
        --max_iterations "${max_iterations}" \
        --min_covering "${min_covering}" \
        --dropout_dim "${dropout_dim}" \
        --dropout_hyp "${dropout_hyp}" \
        --max_failed_attempts "${max_failed_attempts}" \
        --nb_quant_levels "${nb_stairs}" \
        --nb_threads "${nb_threads_used}" \
        --min_fidelity "${min_fidelity}" \
        --seed "${sd}" \
        >>"$REDIRECTION"
        code=$?
        assert_success "Intermediate heuristic #${h} execution #${i}" $code
    done

    f1="${TESTS_DIR}test_same_seed_1.rls"
    f2="${TESTS_DIR}test_same_seed_2.rls"

    difference=$(diff ${f1} ${f2})
    code=0
    if [ "$difference" != "" ]
    then
        code=1
    fi

    assert_success "Same seed, same output equality" $code
}


# main execution
SECONDS=0

test_help_output
test_train_data_file
test_train_predictions_file
test_true_classes_file
test_weights_file
test_output_rules_file
test_heuristic_arg
test_attributes_nb_arg
test_classes_nb_arg

test_heuristics
test_nb_threads
test_same_seed_same_output 1
test_same_seed_same_output 1
test_same_seed_same_output 1
test_same_seed_same_output 2
test_same_seed_same_output 2
test_same_seed_same_output 2

for i in {1..250}
do
    random_test_generation $i
done

print_bold "[$SUCCESS/$TOTAL] tests passed within ${SECONDS} seconds."


# -------------------------------------------------
# Obligatory parameters :
# --train_data_file <train data file>
# --train_data_file <train prediction file>
# --train_class_file <train true class file, not mendatory if classes are specified in train data file>
# --weights_file <weights file. In case of bagging, put prefix of files, ex: dimlpBT, files need to be in the form dimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with --nb_dimlp_nets> [Not mendatory if a rules file is given with -f]
# -f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given]
# --global_rules_outfile <Rules output file>
# --heuristic <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo>
# --nb_attributes <number of attributes>
# --nb_classes <number of classes><Options>

# Options are:
# -S <Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder>
# --nb_dimlp_nets <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>
# --attributes_file <file of attributes>
# -r <file where you redirect console result>
# --max_iterations <max iteration number (100 by default)>
# --min_covering <minimum covering number (2 by default)>
# --dropout_dim <dimension dropout parameter>
# --dropout_hyp <hyperplane dropout parameter>
# --max_failed_attempts <Maximum number of failed attempts to find a Fidex rule when the covering is 1 and the covering strategy is used (30 by default)>
# --nb_quant_levels <number of stairs in staircase activation function (50 by default)>
# -t <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>
# -x <index of positive class for the usage of decision threshold (None by default, 0 for first one)>
# --nb_threads <number of threads used for computing the algorithm (default=1, this means by default its a sequential execution)>
# --min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (default=1)>
# --seed <seed (0=random, default)>
# -------------------------------------------------
