/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#ifndef INCLUDE_CACHE_ICACHEMANAGER_H_
#define INCLUDE_CACHE_ICACHEMANAGER_H_

#include <string>

#include "libtusclient.h"

namespace TUS{
    class TUSFile;

    /**
     * @class ICacheManager
     * @brief Interface for managing cache operations for TUS files.
     *
     * This interface provides methods to store, retrieve, and remove TUS files
     * from the cache.
     */

    class LIBTUSAPI_EXPORT ICacheManager
    {
    public:
        /**
         * @brief Store a TUS file in the cache.
         *
         * @param file A shared pointer to the TUSFile to be stored.
         */
        virtual void store(std::shared_ptr<TUSFile> file) = 0;

        /**
         * @brief Retrieve a TUS file from the cache.
         *
         * @param filePath The path of the file to be retrieved.
         * @param appName The name of the application associated with the file.
         * @param uploadUrl The upload URL associated with the file.
         * @return A shared pointer to the retrieved TUSFile.
         */
        virtual std::shared_ptr<TUSFile> retrieve(std::string filePath, std::string appName, std::string uploadUrl) = 0;

        /**
         * @brief Remove a TUS file from the cache.
         *
         * @param filePath The path of the file to be removed.
         * @param appName The name of the application associated with the file.
         * @param uploadUrl The upload URL associated with the file.
         */
        virtual void remove(std::string filePath, std::string appName, std::string uploadUrl) = 0;
    };
}


#endif // INCLUDE_CACHE_CACHE_RECORD_H_