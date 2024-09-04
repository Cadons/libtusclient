#ifndef REQUEST_H
#define REQUEST_H
#include <string>
#include <map>
#include "libtusclient.h"
#include <functional>
using namespace std;

namespace TUS
{
    namespace Http
    {
        enum class LIBTUSAPI_EXPORT HttpMethod
        {
            GET,
            POST,
            PUT,
            PATCH,
            DELETE,
            HEAD,
            OPTIONS
        };
        /**
         * @brief Represents a HTTP request
         */
        class LIBTUSAPI_EXPORT Request
        {
        public:
            using SuccessCallback = std::function<void(std::string header, std::string data)>;
            using ErrorCallback = std::function<void(std::string header, std::string data)>;
            Request();
            Request(std::string url);
            Request(std::string url, std::string body);
            Request(std::string url, std::string body, HttpMethod method);
            Request(std::string url, std::string body, HttpMethod method, map<string, string> headers);
            Request(std::string url, std::string body, HttpMethod method, map<string, string> headers, SuccessCallback onSuccessCallback, ErrorCallback onErrorCallback);
            Request(const Request &request);
            Request& operator=(const Request& request);
            /**
             * @brief Add a header to the request
             * @param header The header to be added
             */
            void addHeader(std::string key, std::string value);
            /**
             * @brief Add the Authorization header to the request
             * @param berearToken The token to be added to the header
             */
            void autorizationHeader(std::string berearToken);
            std::string getUrl() const;
            std::string getBody() const;
            HttpMethod getMethod() const;
            map<string, string> getHeaders() const;
            void setOnSuccessCallback(SuccessCallback onSuccessCallback);
            void setOnErrorCallback(ErrorCallback onErrorCallback);
            SuccessCallback getOnSuccessCallback() const;
            ErrorCallback getOnErrorCallback() const;
           

        private:
            std::string url;
            std::string body;
            HttpMethod method;
            map<string, string> headers;
            SuccessCallback m_onSuccessCallback;
            ErrorCallback m_onErrorCallback;


            static SuccessCallback defaultSuccessCallback();
            static ErrorCallback defaultErrorCallback();

        };
    }
}

#endif // REQUEST_H