#ifndef DIMLPTRNFCT_H
#define DIMLPTRNFCT_H

#include "checkFun.h"
#include "dimlp.h"
#include "realHyp2.h"
#include "writeErr.h"

#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <vector>

static void GiveAllParam();
static void SaveOutputs(DataSet &data, Dimlp *net, int nbOut, int nbWeightLayers, char *outfile);
int dimlpTrn(string command);

#endif
