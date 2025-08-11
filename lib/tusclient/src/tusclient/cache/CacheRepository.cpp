/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

#include "cache/CacheRepository.h"

#include <iostream>

using json = nlohmann::json;

using TUS::Cache::CacheRepository;
using TUS::Cache::TUSFile;

CacheRepository::CacheRepository(std::string appName, bool clear)
    : m_appName(std::move(appName)), m_path(std::filesystem::temp_directory_path() / m_appName / ".cache.json") {
    if (!std::filesystem::exists(m_path.parent_path())) {
        std::filesystem::create_directories(m_path.parent_path());
    }

}
std::shared_ptr<CacheRepository> CacheRepository::create(std::string appName, bool clear) {
    auto repository = std::make_shared<CacheRepository>(std::move(appName), clear);
    if (clear) {
        repository->clearCache();
    } else {
        repository->open();
    }
    return repository;
}
CacheRepository::~CacheRepository() {
    CacheRepository::save();
}

void CacheRepository::add(std::shared_ptr<TUSFile> item) {
    m_cache.push_back(std::make_shared<TUSFile>(item));
}

void CacheRepository::remove(std::shared_ptr<TUSFile> item) {
    auto it = std::ranges::find_if(m_cache, [&item](const std::shared_ptr<TUSFile> &file) {
        return file->getIdentificationHash() == item->getIdentificationHash();
    });
    //remove folder from temp with item uuid
    std::filesystem::remove_all(
        std::filesystem::temp_directory_path() / m_appName / "files" / boost::uuids::to_string(item->getUuid()));
    if (it != m_cache.end()) {
        m_cache.erase(it);
    }
}

std::shared_ptr<TUSFile> CacheRepository::findByHash(const std::string &id) const {
    if (auto it = std::ranges::find_if(m_cache, [&id](const std::shared_ptr<TUSFile> &file) {
        return file->getIdentificationHash() == id;
    }); it != m_cache.end()) {
        return *it;
    }

    return nullptr;
}

std::vector<std::shared_ptr<TUSFile> > CacheRepository::findAll() const {
    std::vector<std::shared_ptr<TUSFile> > files;

    files.reserve(m_cache.size());
    for (const auto &file: m_cache) {
        files.push_back(file);
    }

    return files;
}

bool CacheRepository::open() {
    if (!std::filesystem::exists(m_path)) {
        return true;
    }

    std::ifstream file(m_path);

    if (!file.is_open()) {
        return false;
    }

    if (!m_cache.empty()) {
        m_cache.clear();
    }
    if (file.peek() == std::ifstream::traits_type::eof()) {
        return true;
    }

    json j;
    file >> j;
    if (!j.is_array() || j.empty()) {
        return true;
    }

    std::vector<std::string> requiredFields = {
        "uuid", "lastEdit", "hash", "filePath", "appName", "uploadUrl",
        "uploadOffset", "resumeFrom", "tusId", "chunkNumber"
    };
    bool completeObject = true;
    for (const auto &item: j) {
        for (const auto &field: requiredFields) {
            if (item.find(field) == item.end()) {
                completeObject = false;
                break;
            }
        }
        if (!completeObject) {
            continue;
        }
        std::string filePath = item["filePath"];
        if (!std::filesystem::exists(filePath)) {
            continue;
        }
        std::string appName = item["appName"];
        std::string uploadUrl = item["uploadUrl"];
        boost::uuids::string_generator gen;
        std::string uuidString = item["uuid"];
        boost::uuids::uuid uuid = gen(uuidString);
        auto tusFile = std::make_shared<TUSFile>(filePath, uploadUrl, appName, uuid);
        tusFile->setUploadOffset(item["uploadOffset"]);
        tusFile->setResumeFrom(item["resumeFrom"]);
        tusFile->setLastEdit(item["lastEdit"]);
        tusFile->setTusIdentifier(item["tusId"]);
        tusFile->setChunkNumber(item["chunkNumber"]);
        m_cache.push_back(tusFile);
    }

    return true;
}

void CacheRepository::clearCache() {
    m_cache.clear();
    save();
    open();
}

bool CacheRepository::save() noexcept {
    try {
        json j;
        if (!m_cache.empty()) {
            for (const auto &file: m_cache) {
                json item;
                item["uuid"] = boost::uuids::to_string(file->getUuid());
                item["lastEdit"] = file->getLastEdit();
                item["hash"] = file->getIdentificationHash();
                item["filePath"] = file->getFilePath();
                item["appName"] = file->getAppName();
                item["uploadUrl"] = file->getUploadUrl();
                item["uploadOffset"] = file->getUploadOffset();
                item["resumeFrom"] = file->getResumeFrom();
                item["tusId"] = file->getTusIdentifier();
                item["chunkNumber"] = file->getChunkNumber();
                j.push_back(item);
            }
        } else {
            j = json::array();
        }

        std::ofstream file(m_path);

        if (!file.is_open()) {
            return false;
        }

        file << j.dump();

        file.close();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error saving cache: " << e.what() << std::endl;
        return false;
    }
}
