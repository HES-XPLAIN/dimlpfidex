#include "checkFun.h"

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string represents a valid integer (positive or negative).
 *
 * @param str A C-style string representing the number to be checked.
 * @return bool Returns true if the string is a valid integer, false otherwise.
 */
bool checkInt(const char *str) {
  // Check if empty
  if (str == nullptr || *str == '\0') {
    return 0;
  }

  int i = 0;

  // Allow - sign for negatives
  if (str[0] == '-') {
    // If it's only a -
    if (str[1] == '\0') {
      return false;
    }
    i = 1; // Start checking from second character
  }

  char ch;
  for (; str[i] != '\0'; i++) {
    ch = str[i];
    if (ch < '0' || ch > '9') {
      return false; // Non numerical character found
    }
  }

  return true;
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string represents a valid positive integer.
 *
 * @param str A C-style string representing the number to be checked.
 * @return bool Returns true if the string is a valid positive integer, false otherwise.
 */
bool checkPositiveInt(const char *str)

{
  char ch;

  for (int i = 0; str[i] != '\0'; i++) {
    ch = str[i];

    if ((ch > '9') || (ch < '0')) {
      return false;
    }
  }

  return true;
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string represents a valid floating-point number.
 *
 * @param str A C-style string representing the number to be checked.
 * @return bool Returns true if the string is a valid float, false otherwise.
 */
bool checkFloatFid(const char *str)

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
      return false;
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
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string is a valid representation of a boolean value.
 *
 * @param input A string input representing a boolean.
 * @return bool Returns true if the string is 'true', 'false', '0', or '1' (case-insensitive), false otherwise.
 */
bool checkBool(std::string input) {
  std::transform(input.begin(), input.end(), input.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return (input == "true" || input == "false" || input == "0" || input == "1");
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string is in the format of a list of floating-point numbers.
 *
 * @param input A string representing a list of floats in the format "[a,b,...,c]" without spaces.
 * @return bool Returns true if the string is a valid list of floats, false otherwise.
 */
bool checkList(std::string input) {
  if (input.size() < 3 || input.front() != '[' || input.back() != ']') {
    return false;
  }

  std::istringstream iss(input.substr(1, input.size() - 2)); // remove brackets
  std::string number;

  while (std::getline(iss, number, ',')) {
    if (!checkFloatFid(number.c_str())) {
      return false;
    }
  }

  return true;
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string is empty or contains only whitespace characters.
 *
 * @param line A string to be checked.
 * @return bool Returns true if the string is empty or contains only whitespace, false otherwise
 */
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

/**
 * @brief Converts a double value to a string and formats it by removing trailing zeros and decimal point if necessary.
 *
 * @param number The double number to be converted.
 * @return std::string A formatted string representing the number.
 */
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

/**
 * @brief Parses a string representing a list of floats and returns them as a vector.
 *
 * @param str A string in the format "[a,b,...,c]" without spaces representing a list of floats.
 * @return std::vector<float> A vector of floats parsed from the string.
 */
std::vector<float> getFloatVectorFromString(const std::string &str) {
  std::vector<float> result;
  auto tokens = splitString(str.substr(1, str.size() - 2), ",");

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

/**
 * @brief Parses a string representing a list of integers and returns them as a vector.
 *
 * @param str A string in the format "[a,b,...,c]" without spaces representing a list of integers.
 * @return std::vector<int> A vector of integers parsed from the string.
 */
std::vector<int> getIntVectorFromString(const std::string &str) {
  std::vector<int> result;
  auto tokens = splitString(str.substr(1, str.size() - 2), ",");

  for (const auto &token : tokens) {
    if (checkInt(token.c_str())) {
      result.push_back(std::stoi(token));
    } else {
      throw CommandArgumentException("Error : Invalid integer value in int vector: " + token);
    }
  }

  return result;
}
