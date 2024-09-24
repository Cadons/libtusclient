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
#include <filesystem>
#include <thread>

#include "TusClient.h"
/**
 * @brief These tests are integration tests that test the TusClient class
 */
namespace TUS::Test
{
    std::filesystem::path generateTestFile(int size = 10);
    std::filesystem::path generateSimpleFile();
    TEST(TusClient, clientCreationTest)
    {

        //create text file
        TUS::TusClient client("testapp","http://localhost:8080/files", generateSimpleFile());

        EXPECT_EQ(client.getUrl(), "http://localhost:8080/files");
        EXPECT_EQ(client.getFilePath(), "test.txt");
        EXPECT_EQ(client.status(), TUS::TusStatus::READY);
        std::filesystem::remove("test.txt");
    }

    TEST(TusClient, uploadTest)
    {

        std::filesystem::path testFilePath = generateTestFile(10);
        std::cout << "Test file path: " << testFilePath << std::endl;
        TUS::TusClient client("testapp","http://localhost:8080", testFilePath,100);

        client.upload();

        EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
    }

    TEST(TusClient, pauseTest)
    {
        std::filesystem::path testFilePath = generateTestFile(10);
        std::cout << "Test file path: " << testFilePath << std::endl;
        TUS::TusClient client("testapp","http://localhost:8080", testFilePath,100);

        std::thread uploadThread([&]() {
            client.upload();
        });
        //wait 10 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        client.pause();
        EXPECT_EQ(client.status(), TUS::TusStatus::PAUSED);
        uploadThread.join();
    }
    TEST(TusClient, pauseResumeTest)
    {
        std::filesystem::path testFilePath = generateTestFile(10);
        std::cout << "Test file path: " << testFilePath << std::endl;
        TUS::TusClient client("testapp", "http://localhost:8080", testFilePath, 100);
        std::thread uploadThread([&]() {
            client.upload();
        });

        while (client.progress()<10)
        {
        }

        
        client.pause();  
        std::cout<<"Pause"<<std::endl;

        EXPECT_EQ(client.status(), TUS::TusStatus::PAUSED);
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout<< "Resuming"<<std::endl;
        float progress = client.progress();
        std::thread resumeThread([&]() {
            client.resume();
        });

        while (client.progress() == progress) {
          
        }

        EXPECT_EQ(client.status(), TUS::TusStatus::UPLOADING);

        resumeThread.join();
        uploadThread.join();
    }

    TEST(TusClient, getServerInformationTest)
    {
       
        TUS::TusClient client("testapp","http://localhost:8080/files", generateSimpleFile());
        std::map<std::string, std::string> serverInformation = client.getTusServerInformation();
        EXPECT_EQ(serverInformation["Tus-Resumable"], "1.0.0");
        EXPECT_EQ(serverInformation["Tus-Version"], "1.0.0");
        EXPECT_EQ(serverInformation["Tus-Extension"], "creation,creation-with-upload,termination,concatenation,creation-defer-length");
        std::filesystem::remove("test.txt");
    }

    TEST(TusClient, cancelUpload)
    {
        std::filesystem::path testFilePath = generateTestFile(10);
        std::cout << "Test file path: " << testFilePath << std::endl;
        TUS::TusClient client("testapp","http://localhost:8080", testFilePath,100);

        std::thread uploadThread([&]() {
            client.upload();
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
       
        client.cancel();
        EXPECT_EQ(client.status(), TUS::TusStatus::CANCELED);
        uploadThread.join();
    }

    TEST(TusClient, retryUpload)
    {
        std::filesystem::path testFilePath = generateTestFile(10);
        std::cout << "Test file path: " << testFilePath << std::endl;
        TUS::TusClient client("testapp","http://localhost:8080", testFilePath,100);

        std::thread uploadThread([&]() {
            client.upload();
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client.cancel();
        uploadThread.join();

        EXPECT_EQ(client.status(), TUS::TusStatus::CANCELED);
        client.retry();
        EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
    }

    std::filesystem::path generateSimpleFile()
    {
        std::ofstream testFile("test.txt");
        testFile << "Hello World";
        testFile.close();
        return "test.txt";
    }

    std::filesystem::path generateTestFile(int size )
    {

        // generate random .dat files 10MB each
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (int i = 0; i < size; i++)
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
        libzippp::ZipArchive zipArchive((std::filesystem::current_path() / "test.zip").string());
        zipArchive.open(libzippp::ZipArchive::New);

        for (int i = 0; i < size; i++)
        {
            zipArchive.addFile(std::to_string(i), (std::filesystem::current_path() / (std::to_string(i) + ".dat")).string());
            // remove the .dat files
        }
        zipArchive.close();
        // remove the .dat files
        for (int i = 0; i < size; i++)
        {
            std::filesystem::remove(std::filesystem::current_path()/( std::to_string(i) + ".dat"));
        }
        return std::filesystem::current_path()/ "test.zip";
    }

}