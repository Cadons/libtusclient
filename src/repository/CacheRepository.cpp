#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/string_generator.hpp>

#include "repository/CacheRepository.h"

using json = nlohmann::json;

using TUS::CacheRepository;
using TUS::TUSFile;

CacheRepository::CacheRepository(std::string appName, bool clear)
    : m_appName(appName), m_path(std::filesystem::path(TEMP_DIR) / m_appName / ".cache.json")
{
    if (!std::filesystem::exists(m_path.parent_path()))
    {
        std::filesystem::create_directories(m_path.parent_path());
    }
    if (clear)
    {
        clearCache();
    }
    else
    {
        open();
    }
}

CacheRepository::~CacheRepository()
{
    save();
}

void CacheRepository::add(const TUSFile &item)
{
    m_cache.push_back(std::make_shared<TUSFile>(item));
}

void CacheRepository::remove(const TUSFile &item)
{
    auto it = std::find_if(m_cache.begin(), m_cache.end(), [&item](const std::shared_ptr<TUSFile> &file)
                           { return file->getIdentificationHash() == item.getIdentificationHash(); });

    if (it != m_cache.end())
    {
        m_cache.erase(it);
    }
}

std::shared_ptr<TUSFile> CacheRepository::findByHash(const std::string &id) const
{
    auto it = std::find_if(m_cache.begin(), m_cache.end(), [&id](const std::shared_ptr<TUSFile> &file)
                           { return file->getIdentificationHash() == id; });

    if (it != m_cache.end())
    {
        return *it;
    }

    return nullptr;
}

std::vector<TUSFile> CacheRepository::findAll() const
{
    std::vector<TUSFile> files;

    for (const auto &file : m_cache)
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

    if (m_cache.size() > 0)
    {
        m_cache.clear();
    }

    json j;
    file >> j;
    if (!j.is_array() || j.empty())
    {
        return true;
    }

    for (const auto &item : j)
    {
        std::string filePath = item["filePath"];
        std::string appName = item["appName"];
        std::string uploadUrl = item["uploadUrl"];
        boost::uuids::string_generator gen;
        std::string uuidString=item["uuid"];
        boost::uuids::uuid uuid = gen(uuidString);
        std::shared_ptr<TUSFile> file = std::make_shared<TUSFile>(filePath, uploadUrl, appName, uuid);
        file->setUploadOffset(item["uploadOffset"]);
        file->setResumeFrom(item["resumeFrom"]);
        file->setLastEdit(item["lastEdit"]);
        file->setTusIdentifier(item["tusId"]);
        file->setChunkNumber(item["chunkNumber"]);
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
    if(m_cache.size()>0){
        for (const auto &file : m_cache)
        {
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
    }else{
        j = json::array();
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
