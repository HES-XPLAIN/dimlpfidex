using namespace std;
#include "checkFun.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

extern void getRules(vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> &rules, vector<string> &lines, vector<string> &stringRules, char *rulesFile, int nbTestAttributsbool, bool hasAttributeNames, vector<string> attributeNames, bool hasClassNames, vector<string> classNames);
extern void getActivatedRules(vector<int> &activatedRules, vector<tuple<vector<tuple<int, bool, double>>, int, int, double, double>> *rules, vector<double> *testValues);
