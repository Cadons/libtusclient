#ifndef INCLUDE_LIBTUSCLIENT_H_
#define INCLUDE_LIBTUSCLIENT_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#define TUS_PROTOCOL_VERSION "1.0.0"
#ifdef WIN32
  #include <windows.h>
  #define LIBTUSAPI_EXPORT __declspec(dllexport)
#else
  #define LIBTUSAPI_EXPORT
#endif

#endif // INCLUDE_LIBTUSCLIENT_H_