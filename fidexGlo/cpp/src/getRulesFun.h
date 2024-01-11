#include "../../../fidexCommon/cpp/src/checkFun.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

void getRules(std::vector<std::tuple<std::vector<std::tuple<int, bool, double>>, int, int, double, double>> &rules, std::vector<std::string> &lines, std::vector<std::string> &stringRules, const char *rulesFile, bool hasAttributeNames, const std::vector<std::string> &attributeNames, bool hasClassNames, const std::vector<std::string> &classNames);
void getActivatedRules(std::vector<int> &activatedRules, std::vector<std::tuple<std::vector<std::tuple<int, bool, double>>, int, int, double, double>> *rules, std::vector<double> *testValues);
