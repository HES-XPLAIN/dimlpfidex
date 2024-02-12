#ifndef DIMLPCLSFCT_H
#define DIMLPCLSFCT_H

#include "../../../common/cpp/src/parameters.h"
#include "dimlp.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

void showDimlpClsParams();
void checkDimlpClsParametersLogicValues(Parameters &p);
static void SaveOutputs(DataSet &data, Dimlp *net, int nbOut, int nbWeightLayers, const string &outfile);
int dimlpCls(const std::string &command);

#endif
