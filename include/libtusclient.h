#ifndef INCLUDE_LIBTUSCLIENT_H_
#define INCLUDE_LIBTUSCLIENT_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#ifdef WIN32
  #include <windows.h>
  #define LIBTUSAPI_EXPORT __declspec(dllexport)
#else
  #define LIBTUSAPI_EXPORT
#endif

//temporary directory for the chunks for each os 
#ifdef WIN32
    #define TEMP_DIR R"(C:\Users\AppData\Local\Temp\TUS\)"
#elif __linux__
    #define TEMP_DIR "/tmp/TUS/"

#elif __APPLE__
    #define TEMP_DIR (std::string(getenv("TMPDIR")) + "/TUS/").c_str()
#elif __ANDROID__
    #define TEMP_DIR "/data/local/tmp/TUS/"
#else
    #define TEMP_DIR "/tmp/TUS/"
#endif

#endif // INCLUDE_LIBTUSCLIENT_H_