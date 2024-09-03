#ifndef REQUEST_H
#define REQUEST_H
#include <string>
#include <map>
using namespace std;
namespace TUS{
    namespace Http{
        /**
         * @brief Represents a HTTP request
         */
        class Request
        {
        public:
            Request();
            Request(std::string url);
            Request(std::string url, std::string body);
            Request(std::string url, std::string body, std::string method);
            Request(std::string url, std::string body, std::string method, map<string,string> headers);
            /**
             * @brief Add a header to the request
             * @param header The header to be added
             */
            void addHeader(std::string key,std::string value);
            /**
             * @brief Add the Authorization header to the request
             * @param berearToken The token to be added to the header
             */
            void autorizationHeader(std::string berearToken);
            std::string getUrl() const;
            std::string getBody() const;
            std::string getMethod() const;
        private:
            std::string url;
            std::string body;
            std::string method;
            map<string,string> headers;
        };
    }
}

#endif // REQUEST_H