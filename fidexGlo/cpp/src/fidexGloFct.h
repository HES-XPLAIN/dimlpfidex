#ifndef FIDEXGLOFCT_H
#define FIDEXGLOFCT_H

#include "../../../fidex/cpp/src/fidexFct.h"
#include "../../../fidexCommon/cpp/src/dataSet.h"
#include "../../../fidexCommon/cpp/src/errorHandler.h"
#include "getRulesFun.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string.h>
#include <time.h>
#include <tuple>
#include <vector>

void showParams();
void launchFidex(std::vector<std::string> &lines, const std::string &fidexCommand, const std::string &ruleFile);
int fidexGlo(const std::string &command);

#endif
