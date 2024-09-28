#ifndef _INCLUDE_REPOSITORY_CACHE_REPOSITORY_H_
#define _INCLUDE_REPOSITORY_CACHE_REPOSITORY_H_

#include "libtusclient.h"
#include "repository/IRepository.h"
#include "model/TUSFile.h"

namespace TUS {

    /**
     * @brief The CacheRepository class is a repository for TUSFile objects.
     * The repository stores TUSFile objects in a cache file.
     */

class LIBTUSAPI_EXPORT CacheRepository : public IRepository<TUSFile> {

public:
    CacheRepository(std::string appName,bool clearCache=false);
    ~CacheRepository() override;

    void add(std::shared_ptr<TUSFile>) override;
    void remove(std::shared_ptr<TUSFile>) override;
    std::shared_ptr<TUSFile> findByHash(const std::string& id) const override;
   std::vector<std::shared_ptr<TUSFile>> findAll() const override;

    bool open() override;
    bool save() override;
    void clearCache();

private:
    std::vector<std::shared_ptr<TUSFile>> m_cache;
    const std::string m_appName;
    const std::filesystem::path m_path;

};

} // namespace TUS

#endif // _INCLUDE_REPOSITORY_CACHE_REPOSITORY_H_