// Copyright [year] <Copyright Owner>
#include <gtest/gtest.h>
#include <fstream>
#include "repository/CacheRepository.h"
#include <boost/uuid/uuid_generators.hpp>
#include "model/TUSFile.h"

using TUS::CacheRepository;
using TUS::TUSFile;

class CacheRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up code here.
        //create test.txt file
        std::ofstream file(m_filePath);
        file << "test-app";
        file.close();

        cacheRepository = std::make_unique<CacheRepository>("test-app",true);
        cacheRepository->clearCache();
    }

    void TearDown() override {
        // Clean up code here.
        cacheRepository.reset();
    }
    const std::filesystem::path m_filePath=std::filesystem::current_path()/ "test.txt";
    std::unique_ptr<CacheRepository> cacheRepository;
    const boost::uuids::uuid m_uuid = boost::uuids::random_generator()();
};
TEST_F(CacheRepositoryTest, add) {
    std::shared_ptr<TUSFile> file=std::make_shared<TUSFile>(m_filePath, "http://localhost:1080/upload", "test-app",m_uuid,"1234567890e39484");
    cacheRepository->add(file);
    auto result = cacheRepository->findByHash(file->getIdentificationHash());
    EXPECT_EQ(result->getIdentificationHash(), file->getIdentificationHash());
}

TEST_F(CacheRepositoryTest, remove) {
    std::shared_ptr<TUSFile> file=std::make_shared<TUSFile>(m_filePath, "http://localhost:1080/upload", "test-app",m_uuid,"1234567890e39484");
    cacheRepository->add(file);
    cacheRepository->remove(file);
    auto result = cacheRepository->findByHash(file->getIdentificationHash());
    EXPECT_EQ(result, nullptr);
}

TEST_F(CacheRepositoryTest, findByHash) {
    std::shared_ptr<TUSFile> file=std::make_shared<TUSFile>(m_filePath, "http://localhost:1080/upload", "test-app",m_uuid,"1234567890e39484");
    cacheRepository->add(file);
    auto result = cacheRepository->findByHash(file->getIdentificationHash());
    EXPECT_EQ(result->getIdentificationHash(), file->getIdentificationHash());
}

TEST_F(CacheRepositoryTest, findAll) {
    std::shared_ptr<TUSFile> file=std::make_shared<TUSFile>(m_filePath, "http://localhost:1080/upload", "test-app",m_uuid,"1234567890e39484");
    cacheRepository->add(file);
    auto result = cacheRepository->findAll();
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0]->getIdentificationHash(), file->getIdentificationHash());
}

TEST_F(CacheRepositoryTest, saveAndOpen) {
    std::shared_ptr<TUSFile> file=std::make_shared<TUSFile>(m_filePath, "http://localhost:1080/upload", "test-app",m_uuid,"1234567890e39484");
    EXPECT_EQ(file->getAppName(), "test-app");
    EXPECT_EQ(file->getFilePath(), m_filePath);
    EXPECT_EQ(file->getUploadUrl(), "http://localhost:1080/upload");
    cacheRepository->add(file);
    cacheRepository->save();
    cacheRepository->open();

    auto result = cacheRepository->findAll();
    EXPECT_EQ(result[0]->getAppName(), "test-app");
    EXPECT_EQ(result[0]->getFilePath(), m_filePath);
    EXPECT_EQ(result[0]->getUploadUrl(), "http://localhost:1080/upload");
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0]->getIdentificationHash(), file->getIdentificationHash());
}


