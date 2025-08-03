/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#ifndef INCLUDE_CHUNK_UTILITY_CHUNKUTILITY_H_
#define INCLUDE_CHUNK_UTILITY_CHUNKUTILITY_H_
#include <cstdint>
#include "libtusclient.h"


namespace TUS::Chunk::Utility {
    /**
     * @brief The ChunkUtility class provides utility methods for working with chunks.
     */
    class EXPORT_LIBTUSCLIENT ChunkUtility {
    public:
        /**
         * @brief Get the chunk size in GB.
         * @param size The size of the chunk in GB
         * @return The size of the chunk in byte
         */
        static std::int64_t getChunkSizeFromGB(int size);

        /**
         * @brief Get the chunk size in MB.
         * @param size The size of the chunk in MB
         * @return The size of the chunk in byte
         */
        static std::int64_t getChunkSizeFromMB(int size);

        /**
         * @brief Get the chunk size in KB.
         * @param size The size of the chunk in KB
         * @return The size of the chunk in byte
         *
         * @return The size of the chunk in byte
         */
        static std::int64_t getChunkSizeFromKB(int size);
    };
} // namespace TUS::Chunk::Utility


#endif // INCLUDE_CHUNK_UTILITY_CHUNKUTILITY_H_
