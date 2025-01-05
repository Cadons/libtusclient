/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#ifndef INCLUDE_CHUNK_TUSCHUNK_H_
#define INCLUDE_CHUNK_TUSCHUNK_H_

#include <string>
#include <vector>
#include <cstdint>
#include "libtusclient.h"

namespace TUS {
    namespace Chunk{
    /**
     * @class TUSChunk
     * @brief Represents a chunk of a TUS file.
     *
     * This class represents a chunk of a TUS file. A chunk is a part of a file
     * that is uploaded to the server in a single request.
     */
    class LIBTUSAPI_EXPORT TUSChunk
    {
    public:
        /**
         * @brief Construct a TUSChunk object.
         *
         * @param data The data of the chunk.
         * @param offset The offset of the chunk in the file.
         */
        TUSChunk(std::vector<uint8_t> data, size_t offset);

        /**
         * @brief Get the data of the chunk.
         *
         * @return The data of the chunk.
         */
        std::vector<uint8_t> getData() const;

        /**
         * @brief Get the offset of the chunk in the file.
         *
         * @return The offset of the chunk in the file.
         */
        size_t getChunkSize() const;

    private:
        std::vector<uint8_t> m_data;
        size_t m_chunkSize;
    };
    } // namespace Model

} // namespace TUS

#endif  // INCLUDE_CHUNK_TUSCHUNK_H_