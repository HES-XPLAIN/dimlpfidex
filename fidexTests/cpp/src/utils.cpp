#include "../headers/utils.hpp"

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

void testAssert(const std::string &testName, bool condition) {
  if (condition) {
    std::cout << GREEN << "[" << testName << "] "
              << "passed" << RESET << std::endl;
  } else {
    std::cout << RED << "[" << testName << "] "
              << "failed" << RESET << std::endl;
  }
}