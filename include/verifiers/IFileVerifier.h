/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#ifndef INCLUDE_VERIFIERS_IFILEVERIFIER_H_
#define INCLUDE_VERIFIERS_IFILEVERIFIER_H_
#include <string>
#include <filesystem>
#include "libtusclient.h"

using std::string;
using std::filesystem::path;
namespace TUS
{
    namespace FileVerifier
    {
        /**
         * @brief The IFileVerifier class provides an interface for verifying files.
         */
        class EXPORT_LIBTUSCLIENT IFileVerifier
        {
        public:
            virtual ~IFileVerifier() = default;

            /**
             * @brief Computes the hash of a file.
             * @param path The path of the file.
             * @return The hash of the file.
             */
            virtual string hash(const std::vector<uint8_t> &buffer) const = 0;
            /**
             * @brief Verifies the hash of a file using a byte buffer.
             * @param buffer The byte buffer of the file.
             * @param hash The hash of the file.
             * @return True if the hash is valid, false otherwise.
             */
            virtual bool verify(const std::vector<uint8_t> &buffer, const string &hash) const = 0;
        };
    } // namespace Repository
} // namespace TUS

#endif // INCLUDE_REPOSITORY_IFILEVERIFIER_H_