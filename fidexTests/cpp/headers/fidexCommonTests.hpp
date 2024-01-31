#ifndef FIDEXCOMMON_TESTS_H
#define FIDEXCOMMON_TESTS_H

#include "../../../fidexCommon/cpp/src/parameters.h"
#include "utils.hpp"

std::string getInvalidDataTypeExceptionMessage(ParameterCode id, const string &wrongValue, const string &typeName);
std::string getAlreadySetArgumentExceptionMessage(ParameterCode id, const string &value);
std::string getArgumentNotFoundExceptionMessage(ParameterCode id);

void test_parameters_setter();
void test_parameters_default_setter();
void test_parameters_getter();
void test_parameters_arg_parser();
void test_parameters_json_parser();

void run_fidexcommon_test_suite();

#endif // FIDEXCOMMON_TESTS_H