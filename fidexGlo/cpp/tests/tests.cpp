#include "tests.h"

void test_help() {
  string args = getArgs(defaultArgs);
  fidexGloRules(args);
}

int main(int argc, char **argv) {
  test_help();
  return EXIT_SUCCESS;
}
