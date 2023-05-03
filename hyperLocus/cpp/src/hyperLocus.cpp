#include "hyperLocusFct.h"

////////////////////////////////////////////////////////////

int main(int nbParam, char** param)

{
   string command = "hyperLocus";
   if (nbParam >= 1){
      for (int p=1; p<nbParam; p++){
         command += " ";
         command += param[p];
      }
   }
   hyperLocus(command);

}
