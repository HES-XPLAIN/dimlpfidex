/**
 * @file
 * @ingroup Executables
 * @brief Entry point for the densCls executable.
 *
 * This file contains the main function which serves as the entry point for the densCls executable.
 */

#include "densClsFct.h"

////////////////////////////////////////////////////////////

/**
 * @brief Entry point for densCls executable.
 *
 * This function serves as the entry point for densCls executable. It constructs a string command
 * from the command line arguments and then calls the densCls function with this command.
 *
 * @param nbParam Number of command line arguments.
 * @param param Array of command line arguments.
 * @return Exit status of the program.
 */
int main(int nbParam, char **param)

{
  std::string command = "";
  if (nbParam >= 1) {
    for (int p = 1; p < nbParam; p++) {
      command += " ";
      command += param[p];
    }
  }
  return densCls(command);
}
