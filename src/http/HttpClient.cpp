#include "http/HttpClient.h"
#include "http/Request.h"
#include <thread>

using namespace TUS::Http;

HttpClient::HttpClient()
{
}

HttpClient::~HttpClient()
{
}
string HttpClient::convertHttpMethodToString(HttpMethod method)
{
    switch (method)
    {
    case HttpMethod::GET:
        return "GET";
    case HttpMethod::POST:
        return "POST";
    case HttpMethod::PUT:
        return "PUT";
    case HttpMethod::PATCH:
        return "PATCH";
    case HttpMethod::DELETE:
        return "DELETE";
    default:
        return "GET";
    }
}
void HttpClient::setupCURLRequest(CURL *curl, HttpMethod method ,Request request)
{
    string methodStr=convertHttpMethodToString(method);
   
    curl_easy_setopt(curl, CURLOPT_URL, request.getUrl().c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, methodStr.c_str());
    struct curl_slist *headers = NULL;
    for (auto const &header : request.getHeaders())
    {
        headers = curl_slist_append(headers, (header.first + ": " + header.second).c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

void HttpClient::get(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl,HttpMethod::GET, request);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    

}

void HttpClient::post(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl,HttpMethod::POST, request);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.getBody().c_str());
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

}

void HttpClient::put(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl,HttpMethod::PUT, request);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.getBody().c_str());
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

void HttpClient::patch(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl,HttpMethod::PATCH, request);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.getBody().c_str());
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

void HttpClient::del(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl,HttpMethod::DELETE, request);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

