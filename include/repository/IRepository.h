#ifndef _INCLUDE_REPOSITORY_IREPOSITORY_H_
#define _INCLUDE_REPOSITORY_IREPOSITORY_H_

#include <string>
#include <vector>
#include <memory>

#include "libtusclient.h"

namespace TUS
{
    namespace Repository
    {
        template <typename T>
        class LIBTUSAPI_EXPORT IRepository
        {
        public:
            virtual ~IRepository() = default;

            virtual void add(std::shared_ptr<T>) = 0;
            virtual void remove(std::shared_ptr<T>) = 0;
            virtual std::shared_ptr<T> findByHash(const std::string &id) const = 0;
            virtual std::vector<std::shared_ptr<T>> findAll() const = 0;
            virtual bool open() = 0;
            virtual bool save() = 0;
        };
    } // namespace Repository

} // namespace TUS
#endif // _INCLUDE_REPOSITORY_IREPOSITORY_H_