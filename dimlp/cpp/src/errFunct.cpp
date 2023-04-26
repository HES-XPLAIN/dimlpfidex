using namespace std;
///////////////////////////////////////////////////////////////////

float Lmse(int nbTar, float netOut[], float target[])

{  int   i;
   float diff, sum = 0.0;

   for (i=0; i<nbTar; i++)
   {
       diff = target[i] - netOut[i];
       sum += diff*diff;
   }

   return sum;
}

///////////////////////////////////////////////////////////////////
