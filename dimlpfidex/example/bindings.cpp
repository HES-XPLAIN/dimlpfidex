#include <pybind11/pybind11.h>
#include <string>

std::string hello();
int add(int i, int j);
long fib(long n);

PYBIND11_MODULE(example, m) {
  m.def("cpp_hello", &hello, "A function that returns a greeting.");
  m.def("cpp_add", &add, "A function that adds two numbers");
  m.def("cpp_fib", &fib, "Give fibonnacci sequence value for a given number.");
}
