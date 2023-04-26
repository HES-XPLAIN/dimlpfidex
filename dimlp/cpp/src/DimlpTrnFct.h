#ifndef DIMLPTRNFCT_H
#define DIMLPTRNFCT_H

#include "dimlp.h"
#include "realHyp2.h"
#include "checkFun.h"
#include "writeErr.h"

#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>

static void GiveAllParam();
static void SaveOutputs(DataSet& data, Dimlp* net, int nbOut, int nbWeightLayers, char* outfile);
int dimlpTrn(string command);

#endif