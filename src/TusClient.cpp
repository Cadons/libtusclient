/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <iostream>
#include <fstream>
#include "TusClient.h"
#include "http/HttpClient.h"
using TUS::TusStatus;
using TUS::TusClient;

TusClient::TusClient(std::string url, std::string filePath) : m_url(url), m_filePath(filePath), m_status(TusStatus::READY), m_httpClient(std::make_unique<Http::HttpClient>())
{
    m_httpClient = std::make_unique<Http::HttpClient>();
    m_tempDir = TEMP_DIR;

}

TusClient::~TusClient()
{
    
}

void TusClient::upload()
{
    //chunk the file
    m_chunkNumber = divideFileInChunks(m_filePath);

    //post to the server the file
    m_httpClient->post(Http::Request(m_url, m_filePath));


    //patch chunks of the file to the server while chunk is not the last one
    for (int i = 0; i < m_chunkNumber; i++)
    {
    }

    //stop that updaload if the last chunk is uploaded
    stop();
    std::cout << "Uploading file " << m_filePath << " to " << m_url << std::endl;
}

void TusClient::cancel()
{
    //delete the file from the server
    std::cout << "Cancelling upload of file " << m_filePath << std::endl;
}

void TusClient::resume()
{
    //resume the upload
    std::cout << "Resuming upload of file " << m_filePath << std::endl;
}

void TusClient::stop()
{
    //stop the upload, but it can be resumed
    std::cout << "Stopping upload of file " << m_filePath << std::endl;
}

int TusClient::progress()
{
    return 0;
}

TusStatus TusClient::status()
{
    return m_status;
}

void TusClient::retry()
{
    std::cout << "Retrying upload of file " << m_filePath << std::endl;
}

path TusClient::getFilePath() const
{
    return m_filePath;
}

std::string TusClient::getUrl() const
{
    return m_url;
}

int TusClient::divideFileInChunks(path filePath)
{
  std::ifstream inputFile(filePath, std::ios::binary | std::ios::ate);// Open input file in binary mode and seek to the end

    if (!inputFile) {
        std::cerr << "Error: Unable to open input file " << filePath << std::endl;
        return -1;
    }

    // Get the size of the file
    std::streamsize fileSize = inputFile.tellg();// Get the current position in the input file, which is the size of the file
    inputFile.seekg(0, std::ios::beg);// Seek back to the beginning of the file

    // Calculate the number of chunks
    int numChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // Create a buffer to store the chunk data
    std::vector<char> buffer(CHUNK_SIZE);
    int totalBytesRead = 0;
    for (int i = 0; i < numChunks; ++i) {
        std::string outputFilePath = "chunk_" + std::to_string(i) + ".bin";
        std::ofstream outputFile(outputFilePath, std::ios::binary);// Open output file in binary mode

        if (!outputFile) {
            std::cerr << "Error: Unable to create output file " << outputFilePath << std::endl;
            return -1;
        }

        // Read from input file and write to output file
        inputFile.read(buffer.data(), CHUNK_SIZE);
        std::streamsize bytesRead = inputFile.gcount();
        outputFile.write(buffer.data(), bytesRead);

        std::cout << "Created chunk: " << outputFilePath << " with " << bytesRead << " bytes." << std::endl;
        std::cout << "Total bytes read: " << inputFile.tellg() << std::endl;
        totalBytesRead += bytesRead;
    }

    inputFile.close();
    std::cout << "Total bytes read: " << totalBytesRead << std::endl;
    return numChunks;
}

bool TusClient::removeChunkFiles(path filePath)
{
    return remove(filePath);
}