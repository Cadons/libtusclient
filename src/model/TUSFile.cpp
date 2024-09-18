
#include <chrono>
#include <string>
#include <functional>



#include "model/TUSFile.h"

using TUS::TUSFile;

TUSFile::TUSFile(std::string filePath, std::string url, std::string appName, int64_t uploadedBytes, int64_t fileSize)
    : m_filePath(filePath), m_uploadUrl(url), m_appName(appName), m_uploadOffset(uploadedBytes), m_fileSize(fileSize),
    m_identifcationHash(std::to_string(std::hash<std::string>{}(filePath + url + appName)))
{
    m_lastEdit = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

TUSFile::~TUSFile()
{
}

std::string TUSFile::getFilePath() const
{
    return m_filePath;
}

std::string TUSFile::getUploadUrl() const
{
    return m_uploadUrl;
}

std::string TUSFile::getAppName() const
{
    return m_appName;
}

int64_t TUSFile::getFileSize() const
{
    return m_fileSize;
}

int64_t TUSFile::getLastEdit() const
{
    return m_lastEdit;
}

int64_t TUSFile::getUploadOffset() const
{
    return m_uploadOffset;
}

void TUSFile::setUploadOffset(int64_t uploadOffset)
{
    m_uploadOffset = uploadOffset;
    updateFile();
}

void TUSFile::setResumeFrom(int resumeFrom)
{
    m_resumeFrom = resumeFrom;
    updateFile();
}

bool TUSFile::select(std::string filePath, std::string appName, std::string uploadUrl)
{
    return m_identifcationHash == std::to_string(std::hash<std::string>{}(filePath + uploadUrl + appName));
}

void TUSFile::updateFile()
{
    m_lastEdit = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

