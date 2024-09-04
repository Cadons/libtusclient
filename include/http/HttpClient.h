#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include "IHttpClient.h"
#include "libtusclient.h"
#include <curl/curl.h>
#include "Request.h"
#include<functional>
namespace TUS
{
    namespace Http
    {

        /**
         * @brief Represents a HTTP client
         */
        class LIBTUSAPI_EXPORT HttpClient: public IHttpClient
        {
        public:
            HttpClient();
            ~HttpClient();

            void get(Request request) override;
            void post(Request request) override;
            void put(Request request) override;
            void patch(Request request) override;
            void del(Request request) override;

            static string convertHttpMethodToString(HttpMethod method);

        private:
            
            void setupCURLRequest(CURL *curl,HttpMethod method, Request request);
            using onDataReceivedCallback = std::function<size_t>(char *ptr, size_t size, size_t nmemb,
                      void *userdata);
            onDataReceivedCallback m_onDataReceivedCallback;
        
        };
    }
}
#endif // HTTPCLIENT_H