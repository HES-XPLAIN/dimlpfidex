#include "checkFun.h"

/**
 * @brief Checks if a given string represents a valid integer.
 *
 * @param input A string representing the number to be checked.
 * @return Returns true if the string is a valid integer, false otherwise.
 */
bool checkInt(const std::string &input) {
  try {
    std::size_t pos;
    std::stoi(input, &pos);

    if (pos != input.length()) {
      return false; // There are some non-numerical characters
    }
  } catch (const std::invalid_argument &) {
    return false;
  } catch (const std::out_of_range &) {
    return false;
  }
  return true;
}

/**
 * @brief Checks if a given string represents a valid floating-point number.
 *
 * @param str A string representing the number to be checked.
 * @return Returns true if the string is a valid float, false otherwise.
 */
bool checkFloat(const std::string &input) {
  try {
    std::size_t pos;        // To store the position of the last processed character
    std::stod(input, &pos); // Convert string to double

    if (pos != input.length()) {
      return false; // There are unprocessed characters left in the string
    }
  } catch (const std::invalid_argument &) { // Handle invalid arguments
    return false;
  } catch (const std::out_of_range &) { // Handles cases where the value is out of range
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string is a valid representation of a boolean value.
 *
 * @param input A string input representing a boolean.
 * @return Returns true if the string is 'true', 'false', '0', or '1' (case-insensitive), false otherwise.
 */
bool checkBool(const std::string &inputTemp) {
  std::string input = inputTemp;
  std::transform(input.begin(), input.end(), input.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return (input == "true" || input == "false" || input == "0" || input == "1");
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string is in the format of a list of floating-point numbers.
 *
 * @param input A string representing a list of floats in any format in (a,b), [a,b], a,b
 * with or without spaces, with or without commas.
 * @return Returns true if the string is a valid list of floats, false otherwise.
 */
bool checkList(const std::string &str) {
  std::string floatPattern = "(-?\\d+(\\.\\d+)?)";

  std::regex listPattern("(\\[|\\()?(" + floatPattern + ")([ ,]+" + floatPattern + ")*(\\]|\\))?");

  return std::regex_match(str, listPattern);
}

////////////////////////////////////////////////////////

/**
 * @brief Checks if a given string is empty or contains only whitespace characters.
 *
 * @param line A string to be checked.
 * @return Returns true if the string is empty or contains only whitespace, false otherwise.
 */
bool checkStringEmpty(const std::string &line) {
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
 * @return A formatted string representing the number.
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
 * @return A vector containing the substrings obtained by splitting the input string.
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
 * @brief Parses a string representing a list of doubles and returns them as a vector.
 *
 * @param str A string in in any format in (a,b), [a,b], a,b with or without spaces,
 * with or without commas representing a list of floats.
 * @return A vector of doubles parsed from the string.
 */
std::vector<double> getDoubleVectorFromString(std::string str) {
  std::regex floatPattern("(-?\\d+(\\.\\d+)?)");
  std::smatch match;
  std::vector<double> numbers;
  while (std::regex_search(str, match, floatPattern)) {
    numbers.push_back(std::stof(match[0].str()));
    str = match.suffix().str(); // Continue with the rest of the string
  }
  return numbers;
}

//////////////////////////////////////////////////////

/**
 * @brief Parses a string representing a list of integers and returns them as a vector.
 *
 * @param str A string in any format in (a,b), [a,b], a,b with or without spaces,
 * with or without commas. representing a list of integers.
 * @return A vector of integers parsed from the string.
 */
std::vector<int> getIntVectorFromString(std::string str) {
  std::regex floatPattern("(-?\\d+(\\.\\d+)?)");
  std::smatch match;
  std::vector<int> numbers;
  while (std::regex_search(str, match, floatPattern)) {
    if (checkInt(match[0])) {
      numbers.push_back(std::stoi(match[0].str()));
    } else {
      throw CommandArgumentException("Error : Invalid integer value in int vector: " + match[0].str());
    }
    str = match.suffix().str(); // Continue with the rest of the string
  }
  return numbers;
}

//////////////////////////////////////////////////////

/**
 * @brief Parses a normalization file to extract statistical data.
 *
 * This function reads a normalization file and extracts statistical information such as attribute indices, wether mean or median was used for normalization
 * mean/median and standard deviations values.
 * It handles files with either numeric indices or attribute names. The function also checks for consistency in the usage of mean or median
 * across the file and detects duplicate indices.
 *
 * @param normalizationFile The path to the normalization file to be parsed.
 * @param nbAttributes The number of attributes expected in the file.
 * @param attributes Optional list of attribute names. If provided, the function will parse the file based on attribute names instead of numeric indices.
 * @return A tuple containing four elements in the following order:
 *         1. A vector of attribute indices (int).
 *         2. A boolean flag indicating whether the file uses 'median' (true) or 'mean' (false).
 *         3. A vector of mean or median values (double) extracted from the file.
 *         4. A vector of standard deviations values (double) extracted from the file.
 * @throws FileContentError If there is a mismatch in the number of attributes, or if the file format is incorrect.
 * @throws FileNotFoundError If the normalization file cannot be opened or found.
 */
std::tuple<std::vector<int>, bool, std::vector<double>, std::vector<double>> parseNormalizationStats(const std::string &normalizationFile, int nbAttributes, const std::vector<std::string> &attributes) {
  std::vector<int> indices_list;
  std::vector<double> mus;
  std::vector<double> sigmas;
  bool withMedian = false;
  bool withMedian_initialized = false;
  std::set<int> unique_indices;

  if (!attributes.empty() && attributes.size() != nbAttributes) {
    throw FileContentError("Error during parsing of " + normalizationFile + ": The number of attributes is not equal to the length of attributes list.");
  }

  // Create some general regex patterns
  std::string indexPattern = "(";

  for (int i = 0; i < nbAttributes; i++) {
    indexPattern += std::to_string(i);
    if (i < nbAttributes - 1) {
      indexPattern += "|";
    }
  }
  indexPattern += ")";

  std::string attrPattern = "";
  if (!attributes.empty()) {
    attrPattern += "(";
    for (int i = 0; i < nbAttributes; i++) {
      attrPattern += attributes[i];
      if (i < nbAttributes - 1) {
        attrPattern += "|";
      }
    }
    attrPattern += ")";
  }

  std::string floatPattern = "(-?\\d+(\\.\\d+)?)(?=$|[^\\d])"; // We ask that the float is followed either by the end of the line either by a not-number character

  // Create regex patterns for a line
  std::vector<std::pair<std::regex, std::string>> patterns;
  std::regex patternIndices("^" + indexPattern + " : original (mean|median): " + floatPattern + ", original std: " + floatPattern);
  std::string patternIndicesStr = "indexPattern";
  std::regex patternAttributes("^" + attrPattern + " : original (mean|median): " + floatPattern + ", original std: " + floatPattern);
  std::string patternAttributesStr = "attributePattern";
  if (!attributes.empty()) {
    patterns.emplace_back(patternAttributes, patternAttributesStr);
  }
  patterns.emplace_back(patternIndices, patternIndicesStr);

  bool patternError;

  for (const auto &pattern : patterns) {

    std::ifstream file(normalizationFile);
    if (!file) {
      throw FileNotFoundError("Error : file " + std::string(normalizationFile) + " not found or couldn't be opened.");
    }

    patternError = false;
    std::string line;
    while (getline(file, line)) {
      std::string mean_median;
      int index;

      if (line.empty())
        continue;

      std::istringstream iss(line);

      std::smatch matches;

      if (std::regex_search(line, matches, pattern.first)) {
        mean_median = matches[2];
        mus.push_back(stod(matches[3]));
        sigmas.push_back(stod(matches[5]));
        if (pattern.second == patternIndicesStr) {
          index = stoi(matches[1]);
        } else {
          std::string attr = matches[1];
          auto it = std::find(attributes.begin(), attributes.end(), attr);
          if (it == attributes.end()) {
            throw FileContentError("Error in " + normalizationFile + ": Attribute not found.");
          }
          index = static_cast<int>(std::distance(attributes.begin(), it));
        }

      } else {
        patternError = true;
        break;
      }

      indices_list.push_back(index);
      unique_indices.insert(index);

      if (!withMedian_initialized) {
        withMedian = (mean_median == "median");
        withMedian_initialized = true;
      } else if ((withMedian && mean_median != "median") || (!withMedian && mean_median != "mean")) {
        throw FileContentError("Error in " + normalizationFile + ": Inconsistency in using mean or median.");
      }
    }
    if (!patternError) {
      break;
    }
  }

  if (patternError) {
    if (attributes.empty()) {
      throw FileContentError("Error in " + normalizationFile + ": File not in the correct format, maybe you forgot to add the attribute file.");
    } else {
      throw FileContentError("Error in " + normalizationFile + ": File not in the correct format.");
    }
  }

  if (indices_list.size() != unique_indices.size()) {
    throw FileContentError("Error in " + normalizationFile + ": Duplicate indices found.");
  }

  return std::make_tuple(indices_list, withMedian, mus, sigmas);
}

//////////////////////////////////////////////////////

/**
 * @brief Checks if a string contains a space between words.
 *
 * Iterates through the string to find spaces. A space is considered between words if it's not at the
 * start or end of the string and is surrounded by non-space characters. Returns true if such a space is found.
 *
 * @param str The string to check.
 * @return True if a space is found between words.
 * @return False if no space is found between words.
 */
bool hasSpaceBetweenWords(const std::string &str) {
  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == ' ' &&
        i > 0 &&
        i < str.length() - 1 &&
        str[i - 1] != ' ' &&
        str[i + 1] != ' ') {
      return true; // Space found between words
    }
  }
  return false; // No space found between words or only tabs are present
}

//////////////////////////////////////////////////////

/**
 * @brief Checks if a given file or directory is valid.
 *
 *
 * @param path path of the file or directory to be checked.
 * @return Whether the file or directory exists or not.
 */
bool exists(const std::string &path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

//////////////////////////////////////////////////////

/**
 * @brief Gets the OS's default filesystem separator character.
 *
 * @return The separator as a string.
 */
std::string getOSSeparator() {
// define separator depending on OS
#if defined(__unix__) || defined(__APPLE__)
  return "/";
#elif defined(_WIN32)
  return "/";
#endif
}

//////////////////////////////////////////////////////

/**
 * @brief Prints an option and its description aligned at the specified width.
 *
 * If the option length exceeds a specified width, the description is printed
 * on the next line with indentation matching the option part.
 * Otherwise, the option and description are printed on the same line.
 *
 * @param option The command line option to be printed (e.g., "--train_data_file <str>").
 * @param description The description for the command line option.
 * @param width The fixed width for the start of the description part.
 */
void printOptionDescription(const std::string &option, const std::string &description, int width) {
  if (option.length() >= width) {
    std::cout << option << std::endl;
    std::cout << std::setw(width) << " " << description << std::endl;
  } else {
    std::cout << std::left << std::setw(width) << option << description << std::endl;
    std::cout << std::right; // Reinitialize alignment
  }
}

//////////////////////////////////////////////////////

/**
 * @brief Counts the number of networks in the weights file.
 *
 * @param fileWts Reference to the file stream opened for reading the weight file.
 * @return The number of networks found in the file.
 */
int countNetworksInFile(const std::string &weightsFile) {

  std::filebuf buf;

  if (buf.open(weightsFile, std::ios_base::in) == nullptr) {
    throw CannotOpenFileError("Cannot open weights file " + weightsFile);
  }

  std::istream fileWts(&buf);

  std::string line;
  int count = 0;
  while (std::getline(fileWts, line)) {
    if (line.find("Network") != std::string::npos) {
      ++count;
    }
  }
  return count > 0 ? count : 1; // If no "Network" keyword is found, assume there's one network
}

//////////////////////////////////////////////////////

/**
 * @brief Parses a line from a file and converts it into a vector of double values.
 *
 * It can handle data separated by a space, a comma(CSV), a semicolon(;) or a tab.
 *
 *
 * @param str The string to parse.
 * @param fileName The name of the file from which the string was read. Used for error reporting.
 * @return A vector of double values parsed from the string.
 * @throws FileContentError if a token cannot be converted to a double.
 */
std::vector<double> parseFileLine(std::string str, const std::string &fileName) {

  std::vector<double> valuesData;

  std::regex re("([ \\t]+)|[,;]");
  std::sregex_token_iterator first{str.begin(), str.end(), re, -1}; //  -1 makes the regex split, it keeps only what was not matched
  std::sregex_token_iterator last;
  std::vector<std::string> stringTokens{first, last};

  for (const std::string &strToken : stringTokens) {
    try {
      if (!checkStringEmpty(strToken)) {
        valuesData.push_back(std::stod(strToken));
      }
    } catch (const std::invalid_argument &) {
      throw FileContentError("Error : Non number found in file " + fileName + " : " + strToken + ".");
    } catch (const std::out_of_range &) {
      throw FileContentError("Error: Number out of range in file " + fileName + " : " + strToken + ".");
    }
  }

  return valuesData;
}

//////////////////////////////////////////////////////
