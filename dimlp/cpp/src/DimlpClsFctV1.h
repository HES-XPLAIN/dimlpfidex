#ifndef DIMLPCLSFCT_H
#define DIMLPCLSFCT_H

#include "checkFun.h"
#include "dimlp.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

void showDimlpClsParams();
static void SaveOutputs(DataSet &data, Dimlp *net, int nbOut, int nbWeightLayers, const char *outfile);
int dimlpCls(const std::string &command);

#endif
