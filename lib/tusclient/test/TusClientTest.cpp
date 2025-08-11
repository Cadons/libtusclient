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
#include <chrono>
#include <format>
#include "TusClient.h"

/**
 * @brief Integration tests for the TusClient class
 */
namespace TUS::Test {
    class TusClientTest : public ::testing::Test {
    public:
        const std::string URL = "http://localhost:8080/files/";
        Logging::LogLevel logLevel = Logging::LogLevel::_DEBUG_;

        static std::filesystem::path generateTestFile(int size = 10);

        static std::filesystem::path generateSimpleFile();

        void SetUp() override {
            std::cout << "URL: " << URL << std::endl;
        }

        void TearDown() override {
            std::filesystem::remove("test.txt");
            std::filesystem::remove("test.zip");

            for (int i = 0; i < MAX_CLEANUP_FILES; ++i) {
                std::filesystem::remove(std::format("{}.dat", i));
            }
        }
    private:
        const int MAX_CLEANUP_FILES = 1000; // Maximum number of files to clean up
    };

    void waitUpload(const TusClient &client, float perc) {
        perc = std::clamp(perc, 1.0f, 100.0f);
        while (client.progress() < perc) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    std::filesystem::path TusClientTest::generateTestFile(int size) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution dis(0, 255);

        std::vector<std::thread> threads;
        threads.reserve(size);

        for (int i = 0; i < size; ++i) {
            threads.emplace_back([&dis,&gen,i]() {
                std::vector<char> data(1024 * 1024);
                std::ranges::generate(data,[&]() { return static_cast<char>(dis(gen)); });

                std::ofstream datFile(std::format("{}.dat", i), std::ios::binary);
                datFile.write(data.data(), data.size());
            });
        }

        for (auto &thread: threads) thread.join();

        libzippp::ZipArchive zipArchive("test.zip");
        zipArchive.open(libzippp::ZipArchive::New);

        for (int i = 0; i < size; ++i) {
            zipArchive.addFile(std::to_string(i), std::format("{}.dat", i));
        }
        zipArchive.close();

        for (int i = 0; i < size; ++i) {
            std::filesystem::remove(std::format("{}.dat", i));
        }

        return "test.zip";
    }

    std::filesystem::path TusClientTest::generateSimpleFile() {
        std::ofstream testFile("test.txt");
        testFile << "Hello World";
        return "test.txt";
    }

    TEST_F(TusClientTest, clientCreationTest) {
        TUS::TusClient client("testapp", URL, generateSimpleFile());
        EXPECT_EQ(client.getUrl(), URL);
        EXPECT_EQ(client.getFilePath(), "test.txt");
        EXPECT_EQ(client.status(), TUS::TusStatus::READY);
    }

    TEST_F(TusClientTest, uploadSimpleTest) {
        auto path = generateTestFile(1);
        TUS::TusClient client("testapp", URL, path, logLevel);
        client.upload();
        EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
    }

    TEST_F(TusClientTest, uploadTest) {
        auto path = generateTestFile(10);
        TUS::TusClient client("testapp", URL, path, logLevel);
        client.upload();
        EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
    }

    TEST_F(TusClientTest, uploadBigTest) {
        auto path = generateTestFile(100);
        TUS::TusClient client("testapp", URL, path, logLevel);
        client.upload();
        EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
    }

    TEST_F(TusClientTest, pauseTest) {
        auto path = generateTestFile(10);
        TUS::TusClient client("testapp", URL, path, logLevel);
        client.setRequestTimeout(std::chrono::milliseconds(100));

        std::thread uploadThread([&client]() { client.upload(); });
        waitUpload(client, 10);
        client.pause();
        uploadThread.join();

        EXPECT_EQ(client.status(), TUS::TusStatus::PAUSED);
    }

    TEST_F(TusClientTest, pauseResumeTest) {
        auto path = generateTestFile(10);
        TUS::TusClient client("testapp", URL, path, logLevel);
        client.setRequestTimeout(std::chrono::milliseconds(10));

        std::thread uploadThread([&client]() { client.upload(); });
        waitUpload(client, 10);
        client.pause();
        uploadThread.join();

        EXPECT_EQ(client.status(), TUS::TusStatus::PAUSED);

        float progress = client.progress();
        std::thread resumeThread([&client]() { client.resume(); });
        waitUpload(client, progress);
        resumeThread.join();

        EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
    }

    TEST_F(TusClientTest, getServerInformationTest) {
        TUS::TusClient client("testapp", URL, generateSimpleFile());
        auto info = client.getTusServerInformation();
        EXPECT_EQ(info["Tus-Resumable"], "1.0.0");
        EXPECT_EQ(info["Tus-Version"], "1.0.0");
        EXPECT_EQ(info["Tus-Extension"],
                  "creation,creation-with-upload,termination,concatenation,creation-defer-length");
    }

    TEST_F(TusClientTest, cancelUpload) {
        auto path = generateTestFile(10);
        TUS::TusClient client("testapp", URL, path, logLevel);
        client.setRequestTimeout(std::chrono::milliseconds(10));

        std::thread uploadThread([&client]() { client.upload(); });
        waitUpload(client, 10);
        client.cancel();
        uploadThread.join();
        EXPECT_EQ(client.status(), TUS::TusStatus::CANCELED);
    }

    TEST_F(TusClientTest, retryUpload) {
        auto path = generateTestFile(10);
        TUS::TusClient client("testapp", URL, path, logLevel);
        client.setRequestTimeout(std::chrono::milliseconds(10));

        std::thread uploadThread([&client]() { client.upload(); });
        waitUpload(client, 10);
        client.cancel();
        uploadThread.join();

        EXPECT_EQ(client.status(), TUS::TusStatus::CANCELED);
        client.retry();

        EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
    }

    TEST_F(TusClientTest, sanitizeUrl) {
        TUS::TusClient client("testapp", "http://test.com", generateSimpleFile());
        EXPECT_EQ(client.getUrl(), "http://test.com/");
    }
} // namespace TUS::Test
