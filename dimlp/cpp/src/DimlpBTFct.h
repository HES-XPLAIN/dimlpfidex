#ifndef DIMLPBTFCT_H
#define DIMLPBTFCT_H

#include "bagDimlp.h"
#include "checkFun.h"
#include "realHyp2.h"
#include "writeErr.h"
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <vector>

void GiveAllParamDimlpBT();
int dimlpBT(const std::string &command);

#endif
