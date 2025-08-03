/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#ifndef INCLUDE_CHUNK_IFILECHUNKER_H_
#define INCLUDE_CHUNK_IFILECHUNKER_H_

#include <vector>
#include <string>
#include <filesystem>
#include "libtusclient.h"

using std::string;
using std::filesystem::path;


namespace TUS::Chunk {
    class TUSChunk;
    /**
     * @class IFileChunker
     * @brief Interface for chunking files into smaller parts.
     *
     * This interface provides methods to set and get the file path, chunk the file into parts of a specified size,
     * and retrieve the total number of chunks.
     */
    template<typename T>
    class EXPORT_LIBTUSCLIENT IFileChunker {
    public:
        virtual ~IFileChunker() = default;

        /**
         * @brief Loads the chunks from the file in memory.
         * @return True if the chunks were loaded successfully, false otherwise.
         */
        virtual bool loadChunks() = 0;

        /**
         * @brief Removes the chunk files, this when a chunk is uploaded successfully.
         *
         * @param path The path of the file to remove the chunks from.
         */
        virtual bool removeChunkFiles() = 0;

        /**
         * @brief Gets the temporary directory for storing the chunks.
         * @return The temporary directory for storing the chunks.
         */
        [[nodiscard]] virtual path getTemporaryDir() const = 0;

        /**
         * @brief Gets the file path.
         * @param chunkNumber The number of the chunk.
         * @return The file path.
         */
        [[nodiscard]] virtual string getChunkFilename(int chunkNumber) const = 0;

        /**
         * @brief Divides the file into chunks.
         *
         * @param path The path of the file to divide.
         * @return The number of chunks the file was divided into.
         */

        virtual int chunkFile() = 0;

        /**
         * @brief Reset the chunker to its initial state.
         */
        virtual void clearChunks() = 0;

        /**
         * @brief Gets the chunks.
         * @return The chunks.
         */
        virtual std::vector<T> getChunks() const = 0;

        /**
         * @brief Gets the chunk file path.
         * @return The file path of the chunk.
         */
        [[nodiscard]] virtual path getChunkFilePath(int chunkNumber) const = 0;

        [[nodiscard]] virtual size_t getChunkSize() const = 0;

        [[nodiscard]] virtual int getChunkNumber() const = 0;
    };
} // namespace TUS::Chunk


#endif // INCLUDE_CHUNK_IFILECHUNKER_H_
