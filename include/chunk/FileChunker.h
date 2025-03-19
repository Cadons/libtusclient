/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#ifndef INCLUDE_CHUNK_FILECHUNKER_H_
#define INCLUDE_CHUNK_FILECHUNKER_H_

#include <string>
#include <filesystem>

#include "IFileChunker.h"
#include "verifiers/IFileVerifier.h"

#include "libtusclient.h"


using std::string;
using std::filesystem::path;


namespace TUS::Chunk {
    class TUSChunk;
    /**
     * @brief This class is responsible for chunking a file into multiple chunks.
     * The chunks are stored in a temporary directory and can be loaded from there.
     * The class also provides methods to remove the chunk files and to get the temporary directory.
     * To get the chunks, the loadChunks method must be called and you can get the chunks with the getChunks method,
     * which returns a vector of TUSChunk objects.
     */
    class EXPORT_LIBTUSCLIENT FileChunker : public IFileChunker<TUSChunk>, public FileVerifier::IFileVerifier {
    private:
        const string CHUNK_FILE_NAME_PREFIX = "_chunk_";
        const string CHUNK_FILE_EXTENSION = ".bin";
        const string m_appName;
        const string m_uuid;
        const path m_tempDir;
        const path m_filePath;
        int64_t m_chunkSize;
        std::vector<TUSChunk> m_chunks;
        int m_chunkNumber{};
        std::unique_ptr<FileVerifier::IFileVerifier> m_verifier;

        void calculateChunkSize();

    public:
        FileChunker(string appName, string uuid, path filepath, int chunkSize = 0,
                    std::unique_ptr<FileVerifier::IFileVerifier> verifier = nullptr);

        virtual ~FileChunker();

        bool loadChunks() override;

        bool removeChunkFiles() override;

        path getTemporaryDir() const override;

        string getChunkFilename(int chunkNumber) const override;

        int chunkFile() override;

        void clearChunks() override;

        std::vector<TUSChunk> getChunks() const override;

        path getChunkFilePath(int chunkNumber) const override;

        size_t getChunkSize() const override;

        int getChunkNumber() const override;

    public:
        string hash(const std::vector<uint8_t> &buffer) const override;

        bool verify(const std::vector<uint8_t> &buffer, const string &hash) const override;
    };
} // namespace TUS::Chunk


#endif // INCLUDE_CHUNK_FILECHUNKER_H_
