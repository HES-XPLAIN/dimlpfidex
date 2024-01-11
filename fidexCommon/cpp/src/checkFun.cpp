#include "checkFun.h"

////////////////////////////////////////////////////////

int CheckPositiveInt(const char *str)

{
  char ch;

  for (int i = 0; str[i] != '\0'; i++) {
    ch = str[i];

    if ((ch > '9') || (ch < '0')) {
      return 0;
    }
  }

  return 1;
}

////////////////////////////////////////////////////////

int CheckFloatFid(const char *str)

{
  int i;
  int countDot;
  int countMinus;
  char ch;

  for (i = 0; str[i] != '\0'; i++) {
    ch = str[i];

    if (((ch > '9') || (ch < '0')) && (ch != '.') && (ch != '-')) {
      std::cout << "Problem with float argument (" << str;
      std::cout << ") or with the previous argument." << std::endl;
      return 0;
    }
  }

  for (i = 0, countDot = 0; str[i] != '\0'; i++)
    if (str[i] == '.')
      countDot++;

  for (i = 0, countMinus = 0; str[i] != '\0'; i++)
    if (str[i] == '.')
      countMinus++;

  if ((countDot > 1) || (countMinus > 1)) {
    std::cout << "Problem with float argument (" << str;
    std::cout << ") or with previous argument." << std::endl;
    return 0;
  }

  return 1;
}

////////////////////////////////////////////////////////
bool checkBool(std::string input) {
  std::transform(input.begin(), input.end(), input.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return (input == "true" || input == "false" || input == "0" || input == "1");
}

////////////////////////////////////////////////////////

bool checkStringEmpty(std::string line) {
  if (line.length() == 0) {
    return true;
  } else {
    if (std::any_of(line.begin(), line.end(), [](int c) { return isgraph(c); })) {
      return false;
    }
    return true;
  }
}

///////////////////////////////////////////////////////

std::string formattingDoubleToString(double number) {
  std::string str = std::to_string(number);
  str.erase(str.find_last_not_of('0') + 1, std::string::npos);
  str.erase(str.find_last_not_of('.') + 1, std::string::npos);
  return str;
}

//////////////////////////////////////////////////////

/**
 * @brief Splits a given string into a vector of substrings based on a specified delimiter.
 *
 * This function takes a string and a delimiter string, then iteratively finds and extracts
 * substrings separated by the delimiter. Substrings at the start and end of the input string
 * are handled appropriately to avoid including empty strings in the result.
 *
 * @param str The string to be split.
 * @param delimiter The string used as the delimiter to split the input string.
 * @return std::vector<std::string> A vector containing the substrings obtained by splitting the input string.
 */
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);

  // Loop to find and add new sub-strings
  while (end != std::string::npos) {
    if (start != end) { // Not adding empty strings
      tokens.push_back(str.substr(start, end - start));
    }
    start = end + delimiter.length();
    end = str.find(delimiter, start);
  }

  // Add last string if not empty
  if (start != str.length()) {
    tokens.push_back(str.substr(start));
  }

  return tokens;
}

//////////////////////////////////////////////////////

std::vector<float> getFloatVectorFromString(const std::string &str) {
  std::vector<float> result;
  auto tokens = splitString(str.substr(1, str.size() - 2), ", ");

  for (const auto &token : tokens) {
    try {
      result.push_back(std::stof(token));
    } catch (const std::invalid_argument &) {
      throw CommandArgumentException("Error : Invalid float value in float vector: " + token);
    }
  }

  return result;
}

//////////////////////////////////////////////////////

std::vector<int> getIntVectorFromString(const std::string &str) {
  std::vector<int> result;
  auto tokens = splitString(str.substr(1, str.size() - 2), ",");

  for (const auto &token : tokens) {
    try {
      result.push_back(std::stoi(token));
    } catch (const std::invalid_argument &) {
      throw CommandArgumentException("Error : Invalid integer value in int vector: " + token);
    }
  }

  return result;
}
