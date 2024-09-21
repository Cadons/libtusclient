#ifndef INCLUDE_MODEL_TUSFILE_H_
#define INCLUDE_MODEL_TUSFILE_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <boost/uuid/uuid.hpp>

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
        /**
         * @brief Construct a new TUSFile object.
         * @param filePath The path of the file to be uploaded.
         * @param uploadUrl The URL to which the file will be uploaded.
         * @param appName The name of the application that created the record.
         */
        TUSFile(std::filesystem::path filePath, std::string uploadUrl, std::string appName,boost::uuids::uuid uuid,std::string tusID="");
        ~TUSFile();
        std::filesystem::path getFilePath() const;
        std::string getUploadUrl() const;
        std::string getAppName() const;
        std::string getIdentificationHash() const;
        int64_t getFileSize() const;
        int64_t getLastEdit() const;
        int64_t getUploadOffset() const;
        int getResumeFrom() const;

        std::string getTusIdentifier() const;
        boost::uuids::uuid getUuid() const;
        
        void setLastEdit(int64_t lastEdit);        
        void setUploadOffset(int64_t uploadOffset);
        void setTusIdentifier(std::string tusIdentifier);
        void setResumeFrom(int resumeFrom);

        bool select(std::string filePath, std::string appName, std::string uploadUrl);


    protected:
        int64_t m_lastEdit;/* last time the record was edited in unix time */
        const std::filesystem::path m_filePath;
        const std::string m_uploadUrl;
        const std::string m_appName;
        int64_t m_uploadOffset;/* the offset of the file that has been uploaded */
        int m_resumeFrom;/* the offset from which the upload should resume */
        const int64_t m_fileSize;
        std::string m_tusIdentifier;/* the identifier of the file */
        const boost::uuids::uuid m_uuid;/* the uuid of the file */

        const std::string m_identifcationHash;/* the hash of the file path and the upload url and app name*/

        void updateFile();
    };

} // namespace TUS
#endif // INCLUDE_MODEL_TUSFILE_H_