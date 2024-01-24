#ifndef DIMLPPREDFCT_H
#define DIMLPPREDFCT_H

#include "checkFun.h"
#include "dimlp.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

void GiveAllParamPred();
static void SaveOutputs(DataSet &data, Dimlp *net, int nbOut, int nbWeightLayers, const char *outfile);
int dimlpPred(const std::string &command);

#endif
