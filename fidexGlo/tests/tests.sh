#!/bin/bash

# This file is used to test fidex algo
EXE="bin/fidexGloRules"
BASE_DIR="$HOME/Documents/hepia"
TESTS_DIR="$BASE_DIR/dimlpfidex-tests/"
CURRENT_VERSION_DIR="$BASE_DIR/dimlpfidex/"
DATASET_DIR="data/pre-act/"
DATA_DIR="${DATASET_DIR}data/"
OUT_DIR="${DATASET_DIR}out/"

# utils
RED='\033[0;31m'
GREEN='\033[0;32m'
RESET='\033[0m'

# to choose which version to test
to_test_dir=$CURRENT_VERSION_DIR
full_dir="${to_test_dir}${DATA_DIR}${OUT_DIR}"

nb_attributes=84
nb_classes=2
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

root_directory="${full_dir}"
attributes_file="${DATA_DIR}attributes.txt"
train_data_file="${DATA_DIR}trainDatas.txt"
train_true_classes="${DATA_DIR}trainClasses.txt"
train_pred_file="${OUT_DIR}train.out"
weigths_file="${OUT_DIR}weights.wts"
input_rules_file=""
output_rules_file="${TESTS_DIR}tests_fidexglo_${date_str}.rls"



# redirection type, uncomment what fits your needs
# REDIRECTION="/dev/null"                 # silent
# REDIRECTION="/dev/stdout"               # in console
REDIRECTION="${TESTS_DIR}output.txt"    # in file

echo "Tests ran on $(date "+%d/%m/%y %R")"
echo "$date_str" >> $REDIRECTION

SUCCESS=0
TOTAL=0

# TODO: test root directory arg
# TODO: test 3rd heuristic
# TODO: check whats happening when 16 threads are declared, tests are showing bizarre behaviour...

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
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Train data file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "123" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_train_predictions_file()
{
    test_name="Train predictions file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Train predictions file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "123" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_true_classes_file()
{
    test_name="Train true classes file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Train true classes file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "123" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_weights_file()
{
    test_name="Weights file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Weights file datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "123" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_output_rules_file()
{
    test_name="Output rules file missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_heuristic_arg()
{
    test_name="Heuristic argument missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -O "${output_rules_file}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Heuristic argument datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -O "${output_rules_file}" \
    -M "abc" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_attributes_nb_arg()
{
    test_name="Attributes number argument missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Attributes number argument datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "abc" \
    -b "${nb_classes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

test_classes_nb_arg()
{
    test_name="Classes number argument missing"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code

    test_name="Classes number argument datatype mismatch"
    print_bold "$test_name"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "abc" \
    >>"$REDIRECTION"
    code=$?
    assert_failure "$test_name" $code
}

# test_input_rules_file()
# {
    #TODO: find use case to test this
    # print_bold "Testing -f missing..."
    # "${to_test_dir}${EXE}" \
    # -T "${train_data_file}" \
    # -P "${train_pred_file}" \
    # -C "${train_true_classes}" \
    # -W "${weigths_file}" \
    # -O "${output_rules_file}" \
    # -M "${heuristic}" \
    # -a "${nb_attributes}" \
    # -b "${nb_classes}" \
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
        -T "${train_data_file}" \
        -P "${train_pred_file}" \
        -C "${train_true_classes}" \
        -W "${weigths_file}" \
        -f "${input_rules_file}" \
        -O "${output_rules_file}" \
        -A "${attributes_file}" \
        -M "${heuristic_val}" \
        -d "${dd}" \
        -h "${dh}" \
        -a "${nb_attributes}" \
        -b "${nb_classes}" \
        -i "${max_iterations}" \
        -y "${min_fidelity}" \
        -z "${seed}" \
        -p "${nth}" \
        -m "${max_failed_attempts}" \
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
            -T "${train_data_file}" \
            -P "${train_pred_file}" \
            -C "${train_true_classes}" \
            -W "${weigths_file}" \
            -f "${input_rules_file}" \
            -O "${output_rules_file}" \
            -A "${attributes_file}" \
            -M "${heuristic}" \
            -a "${nb_attributes}" \
            -b "${nb_classes}" \
            -d "${dd}" \
            -p ${nth} \
            -i "${max_iterations}" \
            -y "${min_fidelity}" \
            -h "${dh}" \
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
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    -z 0 \
    >>"$REDIRECTION"

    code=$?
    assert_success "Random seed" $code

    fixed_seed=10
    print_bold "Fixed seed (${fixed_seed})"
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -f "${input_rules_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    -z "$fixed_seed" \
    >>"$REDIRECTION"

    code=$?
    assert_success "Fixed seed" $code
}

test_nb_threads()
{
    print_bold "Different threads"
    for threads in 2 4 8 16
    do
        "${to_test_dir}${EXE}" \
        -T "${train_data_file}" \
        -P "${train_pred_file}" \
        -C "${train_true_classes}" \
        -W "${weigths_file}" \
        -f "${input_rules_file}" \
        -O "${output_rules_file}" \
        -A "${attributes_file}" \
        -M "${heuristic}" \
        -d "${dd}" \
        -h "${dh}" \
        -a "${nb_attributes}" \
        -b "${nb_classes}" \
        -i "${max_iterations}" \
        -y "${min_fidelity}" \
        -z "${seed}" \
        -p "${threads}" \
        -m "${max_failed_attempts}" \
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
    heuristic=$(random_int 1 2)
    nb_threads_used=$(random_int 4 8)
    min_fidelity=$(random_float 0.5 1.0)
    seed=$(random_int 1 10)
    # decision_threshold=-1.0
    # index_positive_class=-1

    # missing -r, -S, -f, -x & -t
    "${to_test_dir}${EXE}" \
    -T "${train_data_file}" \
    -P "${train_pred_file}" \
    -C "${train_true_classes}" \
    -W "${weigths_file}" \
    -O "${output_rules_file}" \
    -M "${heuristic}" \
    -a "${nb_attributes}" \
    -b "${nb_classes}" \
    -N "${nb_dimlp_nets}" \
    -A "${attributes_file}" \
    -i "${max_iterations}" \
    -v "${min_covering}" \
    -d "${dd}" \
    -h "${dh}" \
    -m "${max_failed_attempts}" \
    -Q "${nb_stairs}" \
    -p "${nb_threads_used}" \
    -y "${min_fidelity}" \
    -z "${seed}" \
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
        -T "${train_data_file}" \
        -P "${train_pred_file}" \
        -C "${train_true_classes}" \
        -W "${weigths_file}" \
        -O "${TESTS_DIR}test_same_seed_${i}.rls" \
        -M "${h}" \
        -a "${nb_attributes}" \
        -b "${nb_classes}" \
        -N "${nb_dimlp_nets}" \
        -A "${attributes_file}" \
        -i "${max_iterations}" \
        -v "${min_covering}" \
        -d "${dropout_dim}" \
        -h "${dropout_hyp}" \
        -m "${max_failed_attempts}" \
        -Q "${nb_stairs}" \
        -p "${nb_threads_used}" \
        -y "${min_fidelity}" \
        -z "${sd}" \
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

# test_coherence_with_main_version()
# {
#     heur=$1
#     sd=$(random_int 1 1000)
#     print_bold "Coherence with main version"
#     "${CURRENT_VERSION_DIR}${EXE}" \
#     -T "${train_data_file}" \
#     -P "${train_pred_file}" \
#     -C "${train_true_classes}" \
#     -W "${weigths_file}" \
#     -O "${TESTS_DIR}test_coherence_1.rls" \
#     -M "${heur}" \
#     -a "${nb_attributes}" \
#     -b "${nb_classes}" \
#     -N "${nb_dimlp_nets}" \
#     -A "${attributes_file}" \
#     -i "${max_iterations}" \
#     -v "1" \
#     -d "${dropout_dim}" \
#     -h "${dropout_hyp}" \
#     -m "${max_failed_attempts}" \
#     -Q "${nb_stairs}" \
#     -p "${nb_threads_used}" \
#     -y "${min_fidelity}" \
#     -z "${sd}" \
#     >>"$REDIRECTION"
#     code=$?
#     assert_success "1st intermediate execution" $code

#     "${MAIN_VERSION_DIR}${EXE}" \
#     -T "${train_data_file}" \
#     -P "${train_pred_file}" \
#     -C "${train_true_classes}" \
#     -W "${weigths_file}" \
#     -O "${TESTS_DIR}test_coherence_2.rls" \
#     -M "${heur}" \
#     -a "${nb_attributes}" \
#     -b "${nb_classes}" \
#     -N "${nb_dimlp_nets}" \
#     -A "${attributes_file}" \
#     -i "${max_iterations}" \
#     -v "${min_covering}" \
#     -d "${dropout_dim}" \
#     -h "${dropout_hyp}" \
#     -m "${max_failed_attempts}" \
#     -Q "${nb_stairs}" \
#     -p "${nb_threads_used}" \
#     -z "${sd}" \
#     >>"$REDIRECTION"
#     code=$?
#     assert_success "2nd intermediate execution" $code

#     f1="${TESTS_DIR}test_coherence_1.rls"
#     f2="${TESTS_DIR}test_coherence_2.rls"

#     difference=$(diff ${f1} ${f2})
#     code=0
#     if [ "$difference" != "" ]
#     then
#         code=1
#     fi

#     assert_success "Coherence with main version" $code
# }


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

test_dropouts
test_heuristics
test_nb_threads
test_same_seed_same_output 1
test_same_seed_same_output 1
test_same_seed_same_output 1
test_same_seed_same_output 2
test_same_seed_same_output 2
test_same_seed_same_output 2
# test_coherence_with_main_version 1
# test_coherence_with_main_version 2

for i in {1..50}
do
    random_test_generation $i
done

print_bold "[$SUCCESS/$TOTAL] tests passed within ${SECONDS} seconds."


# -------------------------------------------------
# Obligatory parameters :
# -T <train data file>
# -P <train prediction file>
# -C <train true class file, not mendatory if classes are specified in train data file>
# -W <weights file. In case of bagging, put prefix of files, ex: DimlpBT, files need to be in the form DimlpBTi.wts, i=1,2,3,... and you need to specify the number of networks with -N> [Not mendatory if a rules file is given with -f]
# -f <rules file to be converted to hyperlocus> [Not mendatory if a weights file is given]
# -O <Rules output file>
# -M <Heuristic 1: optimal fidexGlo, 2: fast fidexGlo 3: very fast fidexGlo>
# -a <number of attributes>
# -b <number of classes><Options>

# Options are:
# -S <Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>
# -N <number of networks for bagging, 1 means no bagging, necessary to use bagging (1 by default)>
# -A <file of attributes>
# -r <file where you redirect console result>
# -i <max iteration number (100 by default)>
# -v <minimum covering number (2 by default)>
# -d <dimension dropout parameter>
# -h <hyperplan dropout parameter>
# -m <maximum number of failed attempts to find Fidex rule when covering is 1 (30 by default)>
# -Q <number of stairs in staircase activation function (50 by default)>
# -t <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>
# -x <index of positive class for the usage of decision threshold (None by default, 0 for first one)>
# -p <number of threads used for computing the algorithm (default=1, this means by default its a sequential execution)>
# -y <minimal rule fidelity accepted when generating a rule [0,1] (default=1)>
# -z <seed (0=random, default)>
# -------------------------------------------------
