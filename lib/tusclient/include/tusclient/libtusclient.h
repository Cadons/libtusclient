#ifndef INCLUDE_LIBTUSCLIENT_H_
#define INCLUDE_LIBTUSCLIENT_H_

#define TUS_PROTOCOL_VERSION "1.0.0"

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef LIBTUSCLIENT_EXPORTS
    #define EXPORT_LIBTUSCLIENT __declspec(dllexport)
  #else
    #define EXPORT_LIBTUSCLIENT __declspec(dllimport)
  #endif
#else
  #ifdef LIBTUSCLIENT_EXPORTS
    #define EXPORT_LIBTUSCLIENT __attribute__((visibility("default")))
  #else
    #define EXPORT_LIBTUSCLIENT
  #endif
#endif

#endif // INCLUDE_LIBTUSCLIENT_H_