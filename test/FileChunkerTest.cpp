/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include "IFileChunker.h"
#include "FileChunker.h"
#include "model/TUSChunk.h"

class FileChunkerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create a temporary file for testing
        testFilePath = std::filesystem::temp_directory_path() / "testfile.bin";
        std::ofstream testFile(testFilePath, std::ios::binary);
        testFile << std::string(1024 * 1024, 'A'); // 1MB file filled with 'A'
        testFile.close();
    }

    void TearDown() override
    {
        // Remove the temporary file after testing
        std::filesystem::remove(testFilePath);
    }

    std::filesystem::path testFilePath;
};

TEST_F(FileChunkerTest, ChunkFile)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    int chunkCount = chunker->chunkFile();
    EXPECT_GT(chunkCount, 0);

    auto tempDir = chunker->getTUSTempDir();
    for (int i = 0; i < chunkCount; ++i)
    {
        std::filesystem::path chunkFilePath = tempDir / chunker->getChunkFilename(i);
        EXPECT_TRUE(std::filesystem::exists(chunkFilePath));
    }

    chunker->removeChunkFiles();
}

TEST_F(FileChunkerTest, GetChunkFilename)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    std::string chunkFilename = chunker->getChunkFilename(0);
    EXPECT_EQ(chunkFilename, "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e_chunk_0.bin");
}

TEST_F(FileChunkerTest, GetTUSTempDir)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    std::filesystem::path tempDir = chunker->getTUSTempDir();
    EXPECT_TRUE(std::filesystem::exists(tempDir));
    EXPECT_TRUE(tempDir.string().find("TUS") != std::string::npos);
}

TEST_F(FileChunkerTest, LoadChunks)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    int number = chunker->chunkFile();
    EXPECT_TRUE(chunker->loadChunks());

    auto chunks = chunker->getChunks();
    EXPECT_GT(chunks.size(), 0);
    EXPECT_EQ(chunks.size(), number);

    chunker->removeChunkFiles();
}

TEST_F(FileChunkerTest, RemoveChunkFiles)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    chunker->chunkFile();
    EXPECT_TRUE(chunker->removeChunkFiles());

    auto tempDir = chunker->getTUSTempDir();
    EXPECT_TRUE(std::filesystem::is_empty(tempDir));
}

TEST_F(FileChunkerTest, ClearChunks)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    chunker->chunkFile();
    chunker->loadChunks();
    chunker->clearChunks();

    auto chunks = chunker->getChunks();
    EXPECT_EQ(chunks.size(), 0);

    chunker->removeChunkFiles();
}

TEST_F(FileChunkerTest, GetChunks)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    chunker->chunkFile();
    chunker->loadChunks();
    auto chunks = chunker->getChunks();
    EXPECT_GT(chunks.size(), 0);

    chunker->removeChunkFiles();
}

TEST_F(FileChunkerTest, GetChunkFilePath)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    chunker->chunkFile();
    auto tempDir = chunker->getTUSTempDir();
    std::filesystem::path chunkFilePath = chunker->getChunkFilePath(0);
    EXPECT_EQ(chunkFilePath, tempDir / "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e_chunk_0.bin");

    chunker->removeChunkFiles();
}

// Additional Tests

TEST_F(FileChunkerTest, CalculateChunkSize)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
   
    EXPECT_GT(chunker->getChunkSize(), 0);
}

TEST_F(FileChunkerTest, ConstructorWithChunkSize)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath, 512 * 1024); // 512KB
    EXPECT_EQ(chunker->getChunkSize(), 512 * 1024);
}

TEST_F(FileChunkerTest, ConstructorWithoutChunkSize)
{
    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", testFilePath);
    EXPECT_GT(chunker->getChunkSize(), 0);
}

TEST_F(FileChunkerTest, ChunkFileWithSmallFile)
{
    // Create a small file (less than 5MB)
    std::filesystem::path smallFilePath = std::filesystem::temp_directory_path() / "smallfile.bin";
    std::ofstream smallFile(smallFilePath, std::ios::binary);
    smallFile << std::string(1024 * 1024 * 4, 'B'); // 4MB file filled with 'B'
    smallFile.close();

    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", smallFilePath);
    int chunkCount = chunker->chunkFile();
    EXPECT_GT(chunkCount, 0);

    auto tempDir = chunker->getTUSTempDir();
    for (int i = 0; i < chunkCount; ++i)
    {
        std::filesystem::path chunkFilePath = tempDir / chunker->getChunkFilename(i);
        EXPECT_TRUE(std::filesystem::exists(chunkFilePath));
    }

    chunker->removeChunkFiles();
    std::filesystem::remove(smallFilePath);
}

TEST_F(FileChunkerTest, ChunkFileWithLargeFile)
{
    // Create a large file (greater than 1GB)
    std::filesystem::path largeFilePath = std::filesystem::temp_directory_path() / "largefile.bin";
    std::ofstream largeFile(largeFilePath, std::ios::binary);
    largeFile << std::string(1024 * 1024 * 1024, 'C'); // 1GB file filled with 'C'
    largeFile.close();

    std::unique_ptr<TUS::IFileChunker<TUS::TUSChunk>> chunker = std::make_unique<TUS::FileChunker>("TestApp", "c52cb3d0-2ac4-4eb4-8d3a-2b9919389a2e", largeFilePath);
    int chunkCount = chunker->chunkFile();
    EXPECT_GT(chunkCount, 0);

    auto tempDir = chunker->getTUSTempDir();
    for (int i = 0; i < chunkCount; ++i)
    {
        std::filesystem::path chunkFilePath = tempDir / chunker->getChunkFilename(i);
        EXPECT_TRUE(std::filesystem::exists(chunkFilePath));
    }

    chunker->removeChunkFiles();
    std::filesystem::remove(largeFilePath);
}
