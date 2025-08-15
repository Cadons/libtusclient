/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#ifndef INCLUDE_CACHE_CACHEREPOSITORY_H_
#define INCLUDE_CACHE_CACHEREPOSITORY_H_

#include "libtusclient.h"
#include "repository/IRepository.h"
#include "cache/TUSFile.h"


namespace TUS::Cache {
    /**
     * @brief The CacheRepository class is a repository for TUSFile objects.
     * The repository stores TUSFile objects in a cache file.
     */

    class EXPORT_LIBTUSCLIENT CacheRepository : public Repository::IRepository<TUSFile> {
    public:
        explicit CacheRepository(std::string appName, bool clearCache = false);
        static std::shared_ptr<CacheRepository> create(std::string appName, bool clearCache = false);
        ~CacheRepository() override;

        void add(std::shared_ptr<TUSFile>) override;

        void remove(std::shared_ptr<TUSFile>) override;

        [[nodiscard]] std::shared_ptr<TUSFile> findByHash(const std::string &id) const override;

        [[nodiscard]] std::vector<std::shared_ptr<TUSFile> > findAll() const override;

        bool open() override;

        bool save() noexcept override;

        void clearCache();

    private:
        std::vector<std::shared_ptr<TUSFile> > m_cache;
        const std::string m_appName;
        const std::filesystem::path m_path;
    };
} // namespace TUS::Cache


#endif  // INCLUDE_CACHE_CACHEREPOSITORY_H_
