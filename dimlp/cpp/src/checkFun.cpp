using namespace std;
#include <iostream>

////////////////////////////////////////////////////////

int CheckInt(char* str)

{  int  i;
   char ch;

   for (i=0; str[i] != '\0'; i++)
   {
       ch = str[i];

       if ( (ch > '9') || (ch < '0') )
       {
          cout << "Problem with integer argument (" << str;
          cout << ") or with previous argument.\n";

          return 0;
       }
   }

   return 1;
}

////////////////////////////////////////////////////////

int CheckFloat(char* str)

{  int  i, countDot, countMinus;
   char ch;

   for (i=0; str[i] != '\0'; i++)
   {
       ch = str[i];

       if ( (ch > '9')  || (ch < '0') )
          if ( (ch != '.') && (ch != '-') )
          {
          cout << "Problem with float argument (" << str;
          cout << ") or with previous argument.\n";
             return 0;
          }
   }

   for (i=0, countDot=0; str[i] != '\0'; i++)
      if (str[i] == '.') countDot++;

   for (i=0, countMinus=0; str[i] != '\0'; i++)
      if (str[i] == '.') countMinus++;

   if ( (countDot > 1) || (countMinus > 1) )
   {
          cout << "Problem with float argument (" << str;
          cout << ") or with previous argument.\n";
          return 0;
   }

   return 1;
}

////////////////////////////////////////////////////////
