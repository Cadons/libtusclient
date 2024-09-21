#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "repository/CacheRepository.h"


using json = nlohmann::json;

using TUS::CacheRepository;
using TUS::TUSFile;


CacheRepository::CacheRepository(std::string appName)
    : m_appName(appName), m_path(std::filesystem::current_path()/m_appName/ "cache.json")
{
    if(!std::filesystem::exists(std::filesystem::current_path()/m_appName))
    {
        std::filesystem::create_directory(std::filesystem::current_path()/m_appName);
    }
    open();
}

CacheRepository::~CacheRepository()
{
    save();
}

void CacheRepository::add(const TUSFile& item)
{
    m_cache.push_back(std::make_shared<TUSFile>(item));
}

void CacheRepository::remove(const TUSFile& item)
{
    auto it = std::find_if(m_cache.begin(), m_cache.end(), [&item](const std::shared_ptr<TUSFile>& file) {
        return file->getIdentificationHash() == item.getIdentificationHash();
    });

    if (it != m_cache.end())
    {
        m_cache.erase(it);
    }
}

std::shared_ptr<TUSFile> CacheRepository::findByHash(const std::string& id) const
{
    auto it = std::find_if(m_cache.begin(), m_cache.end(), [&id](const std::shared_ptr<TUSFile>& file) {
        return file->getIdentificationHash() == id;
    });

    if (it != m_cache.end())
    {
        return *it;
    }

    return nullptr;
}

std::vector<TUSFile> CacheRepository::findAll() const
{
    std::vector<TUSFile> files;

    for (const auto& file : m_cache)
    {
        files.push_back(*file);
    }

    return files;
}

bool CacheRepository::open()
{
    if (!std::filesystem::exists(m_path))
    {
        return true;
    }

    std::ifstream file(m_path);

    if (!file.is_open())
    {
        return false;
    }

    if(m_cache.size()>0)
    {
        m_cache.clear();
    }

    json j;
    file >> j;

    for (const auto& item : j)
    {

        std::string filePath = item["filePath"];
        std::string appName = item["appName"];
        std::string uploadUrl = item["uploadUrl"];
        std::shared_ptr<TUSFile> file = std::make_shared<TUSFile>(filePath, uploadUrl,appName);
        file->setUploadOffset(item["uploadOffset"]);
        file->setResumeFrom(item["resumeFrom"]);
        file->setLastEdit(item["lastEdit"]);
        m_cache.push_back(file);
       
    }

    return true;
}

void CacheRepository::clearCache()
{
    m_cache.clear();
    save();
    open();
}

bool CacheRepository::save()
{
    json j;

    for (const auto& file : m_cache)
    {
        json item;

        item["lastEdit"] = file->getLastEdit();
        item["hash"] = file->getIdentificationHash();
        item["filePath"] = file->getFilePath();
        item["appName"] = file->getAppName();
        item["uploadUrl"] = file->getUploadUrl();
        item["uploadOffset"] = file->getUploadOffset();
        item["resumeFrom"] = file->getResumeFrom();
        j.push_back(item);
    }
    std::ofstream file(m_path);

    if (!file.is_open())
    {
        return false;
    }

    file << j.dump();

    file.close();
    return true;
}

