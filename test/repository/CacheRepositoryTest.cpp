#include <gtest/gtest.h>
#include <fstream>
#include "repository/CacheRepository.h"
#include "model/TUSFile.h"

using namespace TUS;

class CacheRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up code here.
        //create test.txt file
        std::ofstream file(m_filePath);
        file << "test-app";
        file.close();

        cacheRepository = std::make_unique<CacheRepository>("test-app");
        cacheRepository->clearCache();
    }

    void TearDown() override {
        // Clean up code here.
        cacheRepository.reset();
    }
    const std::filesystem::path m_filePath=std::filesystem::current_path()/ "test.txt";
    std::unique_ptr<CacheRepository> cacheRepository;
};

TEST_F(CacheRepositoryTest, add) {
    TUSFile file(m_filePath, "http://localhost:1080/upload", "test-app");
    cacheRepository->add(file);
    auto result = cacheRepository->findByHash(file.getIdentificationHash());
    ASSERT_EQ(result->getIdentificationHash(), file.getIdentificationHash());
}

TEST_F(CacheRepositoryTest, remove) {
    TUSFile file(m_filePath, "http://localhost:1080/upload", "test-app");
    cacheRepository->add(file);
    cacheRepository->remove(file);
    auto result = cacheRepository->findByHash(file.getIdentificationHash());
    ASSERT_EQ(result, nullptr);
}

TEST_F(CacheRepositoryTest, findByHash) {
    TUSFile file(m_filePath, "http://localhost:1080/upload", "test-app");
    cacheRepository->add(file);
    auto result = cacheRepository->findByHash(file.getIdentificationHash());
    ASSERT_EQ(result->getIdentificationHash(), file.getIdentificationHash());
}

TEST_F(CacheRepositoryTest, findAll) {
    TUSFile file(m_filePath, "http://localhost:1080/upload", "test-app");
    cacheRepository->add(file);
    auto result = cacheRepository->findAll();
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0].getIdentificationHash(), file.getIdentificationHash());
}

TEST_F(CacheRepositoryTest, saveAndOpen) {
    TUSFile file(m_filePath, "http://localhost:1080/upload", "test-app");
    EXPECT_EQ(file.getAppName(), "test-app");
    EXPECT_EQ(file.getFilePath(), m_filePath);
    EXPECT_EQ(file.getUploadUrl(), "http://localhost:1080/upload");
    cacheRepository->add(file);
    cacheRepository->save();
    cacheRepository->open();

    auto result = cacheRepository->findAll();
    EXPECT_EQ(result[0].getAppName(), "test-app");
    EXPECT_EQ(result[0].getFilePath(), m_filePath);
    EXPECT_EQ(result[0].getUploadUrl(), "http://localhost:1080/upload");
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0].getIdentificationHash(), file.getIdentificationHash());
}


