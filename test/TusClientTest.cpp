/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <random>
#include <libzippp/libzippp.h>

#include "TusClient.h"
namespace TUS::Test
{
    std::filesystem::path generateTestFile();
    TEST(TusClient, clientCreationTest)
    {

        TUS::TusClient client("http://localhost:3000/files", "test.txt");

        EXPECT_EQ(client.getUrl(), "http://localhost:8080/files");
        EXPECT_EQ(client.getFilePath(), "test.txt");
        EXPECT_EQ(client.status(), TUS::TusStatus::READY);
    }

    TEST(TusClient, uploadTest)
    {

        std::filesystem::path testFilePath = generateTestFile();
        std::cout << "Test file path: " << testFilePath << std::endl;
        TUS::TusClient client("http://localhost:8080", testFilePath);

        client.upload();
    }

    std::filesystem::path generateTestFile()
    {

        // generate random .dat files 10MB each
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (int i = 0; i < 10; i++)
        {
            std::string data;
            for (int j = 0; j < 1024 * 1024; j++)
            {
                data.push_back(dis(gen));
            }
            std::ofstream datFile(std::filesystem::current_path()/(std::to_string(i) + ".dat"), std::ios::binary);
            datFile.write(data.c_str(), data.size());
            datFile.close();
        }
        // Zip the files
        libzippp::ZipArchive zipArchive(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + "test.zip");
        zipArchive.open(libzippp::ZipArchive::New);

        for (int i = 0; i < 10; i++)
        {
            zipArchive.addFile(std::to_string(i), std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + std::to_string(i) + ".dat");
            // remove the .dat files
        }
        zipArchive.close();
        // remove the .dat files
        for (int i = 0; i < 10; i++)
        {
            std::filesystem::remove(std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + std::to_string(i) + ".dat");
        }
        return std::filesystem::current_path().string() + std::filesystem::path::preferred_separator + "test.zip";
    }

}