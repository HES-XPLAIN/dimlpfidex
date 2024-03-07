#ifndef DIMLPTRNFCT_H
#define DIMLPTRNFCT_H

#include "../../../common/cpp/src/parameters.h"
#include "dimlp.h"
#include "realHyp2.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unordered_map>
#include <vector>

void showDimlpTrnParams();
void checkDimlpTrnParametersLogicValues(Parameters &p);
static void SaveOutputs(DataSet &data, std::shared_ptr<Dimlp> net, int nbOut, int nbWeightLayers, const string &outfile);
int dimlpTrn(const std::string &command = "");

#endif
