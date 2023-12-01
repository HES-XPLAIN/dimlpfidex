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
