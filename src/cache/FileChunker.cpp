/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include "chunk/FileChunker.h"
#include "chunk/TUSChunk.h"
#include "chunk/utility/ChunkUtility.h"

using TUS::Chunk::FileChunker;
using TUS::Chunk::TUSChunk;

void FileChunker::calculateChunkSize()
{
    
    auto fileSize = std::filesystem::file_size(m_filePath);
    if (fileSize >= Chunk::Utility::ChunkUtility::getChunkSizeFromGB(1))
    {
        m_chunkSize = Chunk::Utility::ChunkUtility::getChunkSizeFromMB(10); //>1GB chunk size 10MB
    }
    else if (fileSize >= Chunk::Utility::ChunkUtility::ChunkUtility::getChunkSizeFromMB(100)) //>100MB chunk size 5MB
    {
        m_chunkSize = Chunk::Utility::ChunkUtility::getChunkSizeFromMB(5);
    }
    else if (fileSize >= Chunk::Utility::ChunkUtility::getChunkSizeFromMB(50)) //>50MB chunk size 2MB
    {
        m_chunkSize = Chunk::Utility::ChunkUtility::getChunkSizeFromMB(2);
    }
    else if (fileSize >= Chunk::Utility::ChunkUtility::getChunkSizeFromMB(10)) //>10MB chunk size 1MB
    {
        m_chunkSize = Chunk::Utility::ChunkUtility::getChunkSizeFromMB(1);
    }
    else if (fileSize < Chunk::Utility::ChunkUtility::getChunkSizeFromMB(5)) // <5MB chunk size 100KB
    {
        m_chunkSize = Chunk::Utility::ChunkUtility::getChunkSizeFromKB(100);
    }
    else // <5MB chunk size 32KB
    {
        m_chunkSize = Chunk::Utility::ChunkUtility::ChunkUtility::getChunkSizeFromKB(32);
    }
}
FileChunker::FileChunker(std::string appName, std::string uuid, std::filesystem::path filepath, int chunkSize)
    : CHUNK_FILE_NAME_PREFIX("_chunk_"), CHUNK_FILE_EXTENSION(".bin"), m_appName(appName), m_uuid(uuid), m_tempDir(std::filesystem::temp_directory_path()/"TUS"), m_filePath(filepath)
{
    if (chunkSize >0)
    {
        m_chunkSize = chunkSize;
    }
    else
    {
        calculateChunkSize();
    }
    std::cout<<"Temporarily directory: "<<m_tempDir<<std::endl;
}
std::filesystem::path FileChunker::getTemporaryDir() const
{
    std::filesystem::path filesTempDir = m_tempDir / m_appName / "files" / m_uuid;

    if (!std::filesystem::exists(filesTempDir))
    {
        std::filesystem::create_directories(filesTempDir);
    }
    return filesTempDir;
}

std::string FileChunker::getChunkFilename(int chunkNumber) const
{
    return m_uuid + CHUNK_FILE_NAME_PREFIX + std::to_string(chunkNumber) + CHUNK_FILE_EXTENSION;
}

std::filesystem::path FileChunker::getChunkFilePath(int chunkNumber) const
{
    return getTemporaryDir() / getChunkFilename(chunkNumber);
}

bool FileChunker::loadChunks()
{
    m_chunks.clear(); // Clear any existing chunks

    for (int i = 0; i < m_chunkNumber; i++)
    {
        std::filesystem::path chunkFilePath = getTemporaryDir() / getChunkFilename(i);

        std::ifstream chunkFile(chunkFilePath, std::ios::binary);
        if (!chunkFile)
        {
            std::cerr << "Error: Unable to open chunk file " << chunkFilePath << std::endl;
            return false;
        }

        chunkFile.seekg(0, std::ios::end);             // Seek to the end of the file to get the size
        std::streamsize chunkSize = chunkFile.tellg(); // Get the current position in the file, which is the size of the file
        chunkFile.seekg(0, std::ios::beg);             // Seek back to the beginning of the file

        std::vector<char> chunkData(chunkSize);
        chunkFile.read(chunkData.data(), chunkSize);
        chunkFile.close();

        TUSChunk chunk(chunkData, chunkSize);
        m_chunks.push_back(chunk);
    }
    return true;
}

int FileChunker::chunkFile()
{
    std::ifstream inputFile(m_filePath, std::ios::binary | std::ios::ate); // Open input file in binary mode and seek to the end

    if (!inputFile)
    {
        std::cerr << "Error: Unable to open input file " << m_filePath << std::endl;
        return -1;
    }

    // Get the size of the file
    std::streamsize fileSize = inputFile.tellg(); // Get the current position in the input file, which is the size of the file
    inputFile.seekg(0, std::ios::beg);            // Seek back to the beginning of the file

    // Calculate the number of chunks
    m_chunkNumber = (fileSize + m_chunkSize - 1) / m_chunkSize;

    // Create a buffer to store the chunk data
    std::vector<char> buffer(m_chunkSize);
    int totalBytesRead = 0;

    for (int i = 0; i < m_chunkNumber; ++i)
    {
        std::filesystem::path outputFilePath = getTemporaryDir() / getChunkFilename(i);
        std::ofstream outputFile(outputFilePath, std::ios::binary); // Open output file in binary mode

        if (!outputFile)
        {
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

bool FileChunker::removeChunkFiles()
{
    std::filesystem::path filesTempDir = getTemporaryDir();
    if (std::filesystem::exists(filesTempDir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(filesTempDir))
        {
            std::filesystem::remove(entry.path());
        }
        return true;
    }
    return false;
}

void FileChunker::clearChunks()
{
    m_chunks.clear();
}

std::vector<TUSChunk> FileChunker::getChunks() const
{
    if(m_chunks.empty())
    {
        throw std::runtime_error("Chunks are empty. Call loadChunks() first.");
    }
    return m_chunks;
}

size_t FileChunker::getChunkSize() const
{
    return m_chunkSize;
}

FileChunker::~FileChunker()
{
   
}