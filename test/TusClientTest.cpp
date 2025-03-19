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

#include "TusClient.h"
/**
 * @brief These tests are integration tests that test the TusClient class
 */
namespace TUS::Test {
	class TusClientTest : public ::testing::Test {
	protected:
		//const std::string URL="http://localhost:8080/files/";
		const std::string URL = "http://localhost:8080/files/";

		static std::filesystem::path generateTestFile(int size = 10);

		static std::filesystem::path generateSimpleFile();

		void SetUp() override {
			// Set up code here.
			std::cout << "URL:" << URL << std::endl;
		}

		void TearDown() override {
			if (std::filesystem::exists("test.txt"))
				std::filesystem::remove("test.txt");
			if (std::filesystem::exists("test.zip"))
				std::filesystem::remove("test.zip");
			// remove all .dat
			for (int i = 0; i < 1000; i++) {
				if (std::filesystem::exists(std::to_string(i) + ".dat"))
					std::filesystem::remove(std::to_string(i) + ".dat");
			}
		}

		Logging::LogLevel logLevel = Logging::LogLevel::_DEBUG_;
	};

	void waitUpload(const TusClient &client, float perc) {
		if (perc > 100) {
			perc = 100;
		} else if (perc <= 0) {
			perc = 1;
		}
		while (client.progress() < perc) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	std::filesystem::path TusClientTest::generateTestFile(int size) {
		// generate random .dat files 10MB each
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);
		std::vector<std::thread> threads;
		threads.reserve(size);
		for (int i = 0; i < size; i++) {
			threads.emplace_back([&, i]() {
				std::string data;
				for (int j = 0; j < 1024 * 1024; j++) {
					data.push_back(dis(gen));
				}
				std::ofstream datFile(std::filesystem::current_path() / (std::to_string(i) + ".dat"), std::ios::binary);
				datFile.write(data.c_str(), data.size());
				datFile.close();
			});
		}
		for (auto &thread: threads) {
			thread.join();
		}
		// Zip the files
		libzippp::ZipArchive zipArchive((std::filesystem::current_path() / "test.zip").string());
		zipArchive.open(libzippp::ZipArchive::New);

		for (int i = 0; i < size; i++) {
			zipArchive.addFile(std::to_string(i),
			                   (std::filesystem::current_path() / (std::to_string(i) + ".dat")).string());
		}
		zipArchive.close();
		// remove the .dat files
		for (int i = 0; i < size; i++) {
			std::filesystem::remove(std::filesystem::current_path() / (std::to_string(i) + ".dat"));
		}
		return std::filesystem::current_path() / "test.zip";
	}

	std::filesystem::path TusClientTest::generateSimpleFile() {
		std::ofstream testFile("test.txt");
		testFile << "Hello World";
		testFile.close();
		return "test.txt";
	}

	TEST_F(TusClientTest, clientCreationTest) {
		// create text file
		TUS::TusClient client("testapp", URL, generateSimpleFile());

		EXPECT_EQ(client.getUrl(), URL);
		EXPECT_EQ(client.getFilePath(), "test.txt");
		EXPECT_EQ(client.status(), TUS::TusStatus::READY);
		std::filesystem::remove("test.txt");
	}

	TEST_F(TusClientTest, uploadSimpleTest) {
		std::filesystem::path testFilePath = generateTestFile(1);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", URL, testFilePath, logLevel);

		client.upload();

		EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
	}

	TEST_F(TusClientTest, uploadTest) {
		std::filesystem::path testFilePath = generateTestFile(10);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", URL, testFilePath, logLevel);

		client.upload();

		EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
	}

	TEST_F(TusClientTest, uploadBigTest) {
		std::filesystem::path testFilePath = generateTestFile(100);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", URL, testFilePath, logLevel);

		client.upload();

		EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
	}

	TEST_F(TusClientTest, pauseTest) {
		std::filesystem::path testFilePath = generateTestFile(10);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", URL, testFilePath, logLevel);
		client.setRequestTimeout(std::chrono::milliseconds(100));

		std::thread uploadThread([&]() { client.upload(); });
		waitUpload(client, 10);
		client.pause();
		uploadThread.join();
		EXPECT_EQ(client.status(), TUS::TusStatus::PAUSED);
	}

	TEST_F(TusClientTest, pauseResumeTest) {
		std::filesystem::path testFilePath = generateTestFile(10);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", URL, testFilePath, logLevel);
		client.setRequestTimeout(std::chrono::milliseconds(10));
		std::thread uploadThread([&]() { client.upload(); });
		waitUpload(client, 10);

		client.pause();
		std::cout << "Pause" << std::endl;


		uploadThread.join();
		EXPECT_EQ(client.status(), TUS::TusStatus::PAUSED);

		std::cout << "Resuming" << std::endl;
		float progress = client.progress();
		std::thread resumeThread([&]() { client.resume(); });

		waitUpload(client, progress);

		resumeThread.join();
	}

	TEST_F(TusClientTest, getServerInformationTest) {
		TUS::TusClient client("testapp", URL, generateSimpleFile());
		std::map<std::string, std::string> serverInformation = client.getTusServerInformation();
		EXPECT_EQ(serverInformation["Tus-Resumable"], "1.0.0");
		EXPECT_EQ(serverInformation["Tus-Version"], "1.0.0");
		EXPECT_EQ(serverInformation["Tus-Extension"],
		          "creation,creation-with-upload,termination,concatenation,creation-defer-length");
	}

	TEST_F(TusClientTest, cancelUpload) {
		std::filesystem::path testFilePath = generateTestFile(10);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", URL, testFilePath, logLevel);
		client.setRequestTimeout(std::chrono::milliseconds(10));

		std::thread uploadThread([&]() { client.upload(); });
		waitUpload(client, 10);
		client.cancel();

		EXPECT_EQ(client.status(), TUS::TusStatus::CANCELED);
		uploadThread.join();
	}

	TEST_F(TusClientTest, retryUpload) {
		std::filesystem::path testFilePath = generateTestFile(10);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", URL, testFilePath, logLevel);
		client.setRequestTimeout(std::chrono::milliseconds(10));

		std::thread uploadThread([&]() { client.upload(); });
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

	TEST_F(TusClientTest, testBehindProxy) {
		std::filesystem::path testFilePath = generateTestFile(1);
		std::cout << "Test file path: " << testFilePath << std::endl;
		TUS::TusClient client("testapp", "http://localhost/tus", testFilePath, logLevel);
		try {
			client.upload();
			EXPECT_EQ(client.status(), TUS::TusStatus::FINISHED);
		} catch ([[maybe_unused]] const std::exception &e) {
			std::cerr << "Warning: Proxy is not available. This test will be skipped." << std::endl;
		}
	}
}
