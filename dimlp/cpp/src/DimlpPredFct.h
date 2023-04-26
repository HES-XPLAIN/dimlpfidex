#ifndef DIMLPPREDFCT_H
#define DIMLPPREDFCT_H

#include "dimlp.h"
#include "checkFun.h"
#include "writeErr.h"
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>


static void GiveAllParam();
static void SaveOutputs(DataSet& data, Dimlp* net, int nbOut, int nbWeightLayers, char* outfile);
int dimlpPred(string command);

#endif
