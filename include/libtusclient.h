#ifndef LIBTUSCLIENT
#define LIBTUSCLIENT


#ifdef WIN32
  #include <windows.h>
  #define LIBTUSAPI_EXPORT __declspec(dllexport)
#else
  #define LIBTUSAPI_EXPORT
#endif


#endif // !LIBTUSCLIENT