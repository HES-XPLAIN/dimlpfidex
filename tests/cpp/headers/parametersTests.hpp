#ifndef PARAMETERS_TESTS_H
#define PARAMETERS_TESTS_H

#include "../../../common/cpp/src/parameters.h"
#include "utils.hpp"
#include <sstream>
#include <string>

std::string getInvalidDataTypeExceptionMessage(ParameterCode id, const std::string &wrongValue, const std::string &typeName);
std::string getAlreadySetArgumentExceptionMessage(ParameterCode id, const std::string &value);
std::string getArgumentNotFoundExceptionMessage(ParameterCode id);

uint testSetter();
uint testDefaultSetter();
uint testGetter();
uint testArgsParser();
uint testJsonParser();

#endif // PARAMETERS_TESTS_H
