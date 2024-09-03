#ifndef IHTTPCLIENT_H
#define IHTTPCLIENT_H
#include "libtusclient.h"
#include <string>
#include <list>
using namespace std;
namespace TUS{
    namespace Http{
    class Request;
    
    class LIBTUSAPI_EXPORT IHttpClient
    {
    public:
        virtual void get(Request request) = 0;
        virtual void post(Request request) = 0;
        virtual void put(Request request) = 0;
        virtual void patch(Request request) = 0;
        virtual void del(Request request) = 0;
    };
    }

}

#endif // IHTTPCLIENT_H