#include <iostream>

using namespace std;

////////////////////////////////////////////////////////

int CheckInt(const char *str)

{
  char ch;

  for (int i = 0; str[i] != '\0'; i++) {
    ch = str[i];

    if ((ch > '9') || (ch < '0')) {
      cout << "Problem with integer argument (" << str;
      cout << ") or with previous argument.\n";

      return 0;
    }
  }

  return 1;
}

////////////////////////////////////////////////////////

int CheckFloat(const char *str)

{
  int countDot = 0;
  int countMinus = 0;
  char ch;

  for (int i = 0; str[i] != '\0'; i++) {
    ch = str[i];

    if ((ch > '9' || ch < '0') && (ch != '.' && ch != '-')) {
      cout << "Problem with float argument (" << str;
      cout << ") or with previous argument.\n";
      return 0;
    }
  }

  for (int i = 0; str[i] != '\0'; i++)
    if (str[i] == '.')
      countDot++;

  for (int i = 0; str[i] != '\0'; i++)
    if (str[i] == '.')
      countMinus++;

  if ((countDot > 1) || (countMinus > 1)) {
    cout << "Problem with float argument (" << str;
    cout << ") or with previous argument.\n";
    return 0;
  }

  return 1;
}

////////////////////////////////////////////////////////
