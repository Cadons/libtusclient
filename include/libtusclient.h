#pragma once

#include <vector>
#include <string>


#ifdef WINDOWS
  #include <windows.h>
  #define LIBTUSAPI_EXPORT __declspec(dllexport)
#else
  #define LIBTUSAPI_EXPORT
#endif

