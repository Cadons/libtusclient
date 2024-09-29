/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#ifndef INCLUDE_FILECHUNKER_H_
#define INCLUDE_FILECHUNKER_H_

#include <string>
#include <filesystem>

#include "IFileChunker.h"

#include "libtusclient.h"

using std::string;
using std::filesystem::path;
namespace TUS
{
    class TUSChunk;
    class LIBTUSAPI_EXPORT FileChunker : public IFileChunker<TUSChunk>
    {
    private:
        const string CHUNK_FILE_NAME_PREFIX = "_chunk_";
        const string CHUNK_FILE_EXTENSION = ".bin";
        const string m_appName;
        const string m_uuid;
        const path m_tempDir;
        const path m_filePath;
        int m_chunkSize;
        std::vector<TUSChunk> m_chunks;
        int m_chunkNumber;

        void calculateChunkSize();

    public:
        FileChunker(string appName, string uuid, path filepath, int chunkSize = 0);
        virtual ~FileChunker();
        bool loadChunks() override;
        bool removeChunkFiles() override;
        path getTUSTempDir() const override;
        string getChunkFilename(int chunkNumber) const override;
        int chunkFile() override;
        void clearChunks() override;
        std::vector<TUSChunk> getChunks() const override;

        path getChunkFilePath(int chunkNumber) const override;

        size_t getChunkSize() const override;
    };

} // namespace TUS

#endif // INCLUDE_FILECHUNKER_H_