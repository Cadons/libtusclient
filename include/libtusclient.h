#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define LIBTUSCLIENT_EXPORT __declspec(dllexport)
#else
  #define LIBTUSCLIENT_EXPORT
#endif

LIBTUSCLIENT_EXPORT void libtusclient();
LIBTUSCLIENT_EXPORT void libtusclient_print_vector(const std::vector<std::string> &strings);
