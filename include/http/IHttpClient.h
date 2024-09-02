#ifndef IHTTPCLIENT_H
#define IHTTPCLIENT_H
#include "libtusclient.h"
#include <string>
#include <list>
using namespace std;
namespace TUS{
    namespace Http{
    class LIBTUSAPI_EXPORT IHttpClient
    {
    public:
        virtual void get(string url,list<string> header) = 0;
        virtual void post(string url,list<string> header, string data) = 0;
        virtual void put(string url,list<string> header, string data) = 0;
        virtual void patch(string url,list<string> header, string data) = 0;
        virtual void del(string url,list<string> header) = 0;
    };
    }

}

#endif // IHTTPCLIENT_H