#ifndef DIMLPCLSFCT_H
#define DIMLPCLSFCT_H

#include "dimlp.h"
#include "checkFun.h"
#include "writeErr.h"
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>

static void GiveAllParam();
static void SaveOutputs(DataSet& data, Dimlp* net, int nbOut,  int nbWeightLayers, char* outfile);
int dimlpCls(string command);

#endif
