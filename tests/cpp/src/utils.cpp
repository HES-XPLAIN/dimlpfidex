#include "../headers/utils.hpp"

std::string getInvalidDataTypeExceptionMessage(ParameterCode id, const std::string &wrongValue, const std::string &typeName) {
  return "CommandArgumentException: Parsing error: argument (ID " + Parameters::getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid " + typeName + ".";
}

std::string getAlreadySetArgumentExceptionMessage(ParameterCode id, const std::string &value) {
  return "CommandArgumentException: Parsing error: argument (ID " + Parameters::getParameterName(id) + ") with value \"" + value + "\" is already set, cannot override it.";
}

std::string getArgumentNotFoundExceptionMessage(ParameterCode id) {
  return "CommandArgumentException: Parameters error: argument (ID " + Parameters::getParameterName(id) + ") requested was not found, try to rerun including it.";
}

std::string getInvalidFileOrDirectoryMessage(ParameterCode id, const std::string &wrongValue) {
  return "CommandArgumentException: Parameters error: argument (ID " + Parameters::getParameterName(id) + ") with value \"" + wrongValue + "\" is not a valid path. The directory or file specified could not be found.";
}

std::string argsToString(const std::map<std::string, std::string> &args) {
  std::stringstream ss;
  for (const auto &kv : args) {
    ss << kv.first << " " << kv.second << " ";
  }

  return ss.str();
}

std::map<std::string, std::string> remove(const std::map<std::string, std::string> &args, const std::vector<std::string> &toRemove) {
  std::map<std::string, std::string> result = args;

  for (std::string item : toRemove) {
    result.erase(item);
  }

  return result;
}

int testExec(const std::string &testName, const std::string &command, int (*fn)(const std::string&)) {
  try {
    int code = fn(command);
    return testAssert(testName, code == 0);
  } catch (std::exception &e) {
    return testAssert(testName, false);
  }
}

int testAssert(const std::string &testName, bool condition) {
  if (condition) {
    std::cout << GREEN << "[" << testName << "] "
              << "passed" << RESET << std::endl;
    return 1;
  } else {
    std::cout << RED << "[" << testName << "] "
              << "failed" << RESET << std::endl;
    return 0;
  }
}