#include "../include/examples.hpp"
#include <string>

std::string hello() {
  return "Hello, C++ World!";
}

int add(int i, int j) {
  return i + j;
}

long fib(long n) {
  if (n < 2) {
    return 1;
  }
  return fib(n - 2) + fib(n - 1);
}
