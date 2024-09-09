/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <iostream>
#include "TusClient.h"
#include "http/HttpClient.h"
using TUS::TusStatus;
using TUS::TusClient;

TusClient::TusClient(std::string url, std::string filePath) : m_url(url), m_filePath(filePath), m_status(TusStatus::READY), m_httpClient(std::make_unique<Http::HttpClient>())
{
   // m_httpClient = std::make_unique<Http::IHttpClient>();
}

TusClient::~TusClient()
{
    
}

void TusClient::upload()
{
    //chunk the file

    //post to the server the file

    //patch chunks of the file to the server while chunk is not the last one

    //stop that updaload if the last chunk is uploaded
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

std::string TusClient::getFilePath() const
{
    return m_filePath;
}

std::string TusClient::getUrl() const
{
    return m_url;
}
