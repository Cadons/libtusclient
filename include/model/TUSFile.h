#ifndef INCLUDE_CACHE_CACHE_RECORD_H_
#define INCLUDE_CACHE_CACHE_RECORD_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
#include <filesystem>
#include "libtusclient.h"
namespace TUS
{

    /**
     * @brief The TUSFile class represents a file upload record in the cache.
     * When a file is uploaded, a record is created and stored in the cache, this
     * needs to be done in order to resume the upload if the upload is interrupted.
     * The record contains the file path, the upload URL, the upload offset,
     * the size of the full file and the application name that created the record.
     * This class provides methods for storing and retrieving data from the cache.
     */
    class LIBTUSAPI_EXPORT TUSFile
    {
    public:
        
        TUSFile(std::string filePath, std::string uploadUrl, std::string appName, int64_t uploadedBytes, int64_t fileSize);
        ~TUSFile();
        std::string getFilePath() const;
        std::string getUploadUrl() const;
        std::string getAppName() const;
        int64_t getFileSize() const;
        int64_t getLastEdit() const;
        int64_t getUploadOffset() const;

        
        void setUploadOffset(int64_t uploadOffset);

        void setResumeFrom(int resumeFrom);

        bool select(std::string filePath, std::string appName, std::string uploadUrl);


    private:
        int64_t m_lastEdit;/* last time the record was edited in unix time */
        const std::string m_filePath;
        const std::string m_uploadUrl;
        const std::string m_appName;
        int64_t m_uploadOffset;/* the offset of the file that has been uploaded */
        int m_resumeFrom;/* the offset from which the upload should resume */
        const int64_t m_fileSize;

        const std::string m_identifcationHash;/* the hash of the file path and the upload url and app name*/

        void updateFile();
    };

} // namespace TUS
#endif // INCLUDE_CACHE_CACHE_RECORD_H_