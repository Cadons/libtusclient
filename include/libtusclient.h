/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#ifndef INCLUDE_LIBTUSCLIENT_H_
#define INCLUDE_LIBTUSCLIENT_H_
#define TUS_PROTOCOL_VERSION "1.0.0"
#ifdef _WINDOWS
#include <windows.h>
    #ifndef EXPORT_ALL
#        if defined(BUILD_SHARED)
           
            // Building the DLL (exporting)
#            define EXPORT_LIBTUSCLIENT __declspec(dllexport)
        #else
            // Using the DLL (importing)
#            define EXPORT_LIBTUSCLIENT __declspec(dllimport)
        #endif
    #else
        #define EXPORT_LIBTUSCLIENT
    #endif
#else
    // Non-Windows platforms (optional, but typically use empty macro or other logic)
    #define EXPORT_LIBTUSCLIENT
#endif

#endif // INCLUDE_LIBTUSCLIENT_H_