#ifndef PARAMETERS_TESTS_H
#define PARAMETERS_TESTS_H

#include "../../../common/cpp/src/parameters.h"
#include "utils.hpp"
#include <sstream>
#include <string>

std::string getInvalidDataTypeExceptionMessage(ParameterCode id, const string &wrongValue, const string &typeName);
std::string getAlreadySetArgumentExceptionMessage(ParameterCode id, const string &value);
std::string getArgumentNotFoundExceptionMessage(ParameterCode id);

void testSetter();
void testDefaultSetter();
void testGetter();
void testArgsParser();
void testJsonParser();

#endif // PARAMETERS_TESTS_H
