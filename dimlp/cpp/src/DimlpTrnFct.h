#ifndef DIMLPTRNFCT_H
#define DIMLPTRNFCT_H

#include "checkFun.h"
#include "dimlp.h"
#include "realHyp2.h"
#include "writeErr.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

void GiveAllParamDimlpTrn();
static void SaveOutputs(DataSet &data, std::shared_ptr<Dimlp> net, int nbOut, int nbWeightLayers, const char *outfile);
int dimlpTrn(const std::string &command);

#endif
