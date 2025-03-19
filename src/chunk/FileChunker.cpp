/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <filesystem>

#include "chunk/FileChunker.h"
#include "chunk/TUSChunk.h"
#include "chunk/utility/ChunkUtility.h"
#include "verifiers/Md5Verifier.h"

using TUS::Chunk::FileChunker;
using TUS::Chunk::TUSChunk;
using TUS::Chunk::Utility::ChunkUtility;

void FileChunker::calculateChunkSize() {
    m_chunkNumber = 0;
    auto fileSize = std::filesystem::file_size(m_filePath);
    if (static_cast<int64_t>(fileSize) >= ChunkUtility::getChunkSizeFromGB(1)) {
        m_chunkSize = ChunkUtility::getChunkSizeFromMB(10);
    } else if (static_cast<int64_t>(fileSize) >= ChunkUtility::getChunkSizeFromMB(100)) {
        m_chunkSize = ChunkUtility::getChunkSizeFromMB(5);
    } else if (static_cast<int64_t>(fileSize) >= ChunkUtility::getChunkSizeFromMB(50)) {
        m_chunkSize = ChunkUtility::getChunkSizeFromMB(2);
    } else if (static_cast<int64_t>(fileSize) >= ChunkUtility::getChunkSizeFromMB(10)) {
        m_chunkSize = static_cast<int>(ChunkUtility::getChunkSizeFromMB(1));
    } else {
        m_chunkSize = std::filesystem::file_size(m_filePath); // no chunking, upload the whole file
        m_chunkNumber = 1;
        return;
    }

    m_chunkNumber = static_cast<int>((fileSize + m_chunkSize - 1) / m_chunkSize);
}

FileChunker::FileChunker(std::string appName, std::string uuid, std::filesystem::path filepath, int chunkSize,
                         std::unique_ptr<FileVerifier::IFileVerifier> verifier)
    : CHUNK_FILE_NAME_PREFIX("_chunk_"), CHUNK_FILE_EXTENSION(".bin"), m_appName(std::move(appName)),
      m_uuid(std::move(uuid)), m_tempDir(std::filesystem::temp_directory_path() / "TUS"),
      m_filePath(std::move(filepath)) {
    if (chunkSize > 0) {
        m_chunkSize = chunkSize;
    } else {
        calculateChunkSize();
    }

    if (verifier == nullptr) {
        m_verifier = std::make_unique<FileVerifier::Md5Verifier>();
    } else {
        m_verifier = std::move(verifier);
    }
}

std::filesystem::path FileChunker::getTemporaryDir() const {
    std::filesystem::path filesTempDir = m_tempDir / m_appName / "files" / m_uuid;

    if (!std::filesystem::exists(filesTempDir)) {
        std::filesystem::create_directories(filesTempDir);
    }
    return filesTempDir;
}

std::string FileChunker::getChunkFilename(int chunkNumber) const {
    return m_uuid + CHUNK_FILE_NAME_PREFIX + std::to_string(chunkNumber) + CHUNK_FILE_EXTENSION;
}

std::filesystem::path FileChunker::getChunkFilePath(int chunkNumber) const {
    return getTemporaryDir() / getChunkFilename(chunkNumber);
}

bool FileChunker::loadChunks() {
    m_chunks.clear(); // Clear any existing chunks

    for (int i = 0; i < m_chunkNumber; i++) {
        std::filesystem::path chunkFilePath = getTemporaryDir() / getChunkFilename(i);

        std::ifstream chunkFile(chunkFilePath, std::ios::binary);
        if (!chunkFile) {
            throw std::runtime_error("Failed to open chunk file: " + chunkFilePath.string());
        }

        chunkFile.seekg(0, std::ios::end); // Seek to the end of the file to get the size
        std::streamsize chunkSize = chunkFile.tellg();
        // Get the current position in the file, which is the size of the file
        chunkFile.seekg(0, std::ios::beg); // Seek back to the beginning of the file

        std::vector<uint8_t> chunkData(chunkSize);
        chunkFile.read(reinterpret_cast<char *>(chunkData.data()), chunkSize);
        chunkFile.close();

        TUSChunk chunk(chunkData, chunkSize);
        m_chunks.push_back(chunk);
    }
    return true;
}

int FileChunker::chunkFile() {
    std::ifstream inputFile(m_filePath, std::ios::binary | std::ios::ate);
    // Open input file in binary mode and seek to the end

    if (!inputFile) {
        throw std::runtime_error("Failed to open input file: " + m_filePath.string());
    }

    // Get the size of the file
    inputFile.seekg(0, std::ios::beg); // Seek back to the beginning of the file

    // Create a buffer to store the chunk data
    std::vector<char> buffer(m_chunkSize);
    std::streamsize totalBytesRead = 0;

    for (int i = 0; i < m_chunkNumber; ++i) {
        std::filesystem::path outputFilePath = getTemporaryDir() / getChunkFilename(i);
        std::ofstream outputFile(outputFilePath, std::ios::binary); // Open output file in binary mode

        if (!outputFile) {
            std::cerr << "Error: Unable to create output file " << outputFilePath << std::endl;
            return -1;
        }

        // Read from input file and write to output file
        inputFile.read(buffer.data(), m_chunkSize);
        std::streamsize bytesRead = inputFile.gcount();
        outputFile.write(buffer.data(), bytesRead);
        totalBytesRead += bytesRead;
    }

    inputFile.close();
    return m_chunkNumber;
}

bool FileChunker::removeChunkFiles() {
    std::filesystem::path filesTempDir = getTemporaryDir();
    if (std::filesystem::exists(filesTempDir)) {
        for (const auto &entry: std::filesystem::directory_iterator(filesTempDir)) {
            std::filesystem::remove(entry.path());
        }
        return true;
    }
    return false;
}

void FileChunker::clearChunks() {
    m_chunks.clear();
}

std::vector<TUSChunk> FileChunker::getChunks() const {
    if (m_chunks.empty()) {
        throw std::runtime_error("Chunks are empty. Call loadChunks() first.");
    }
    return m_chunks;
}

int FileChunker::getChunkNumber() const {
    if (m_chunks.empty()) {
        return 0;
    }
    return m_chunkNumber;
}

size_t FileChunker::getChunkSize() const {
    return m_chunkSize;
}

FileChunker::~FileChunker()
= default;

std::string FileChunker::hash(const std::vector<uint8_t> &buffer) const {
    return m_verifier->hash(buffer);
}

bool FileChunker::verify(const std::vector<uint8_t> &buffer, const std::string &hash) const {
    return m_verifier->verify(buffer, hash);
}
