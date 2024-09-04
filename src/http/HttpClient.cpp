#include "http/HttpClient.h"
#include "http/Request.h"
#include <thread>
#include <iostream>

using namespace TUS::Http;

HttpClient::HttpClient()

{
}

HttpClient::~HttpClient()
{
}
size_t write_data(void *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append((char *)ptr, size * nmemb);
    return size * nmemb;
}
std::string HttpClient::convertHttpMethodToString(HttpMethod method)
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
    case HttpMethod::HEAD:
        return "HEAD";
    case HttpMethod::OPTIONS:
        return "OPTIONS";
    default:
        return "GET";
    }
}
void HttpClient::setupCURLRequest(CURL *curl, HttpMethod method, Request request)
{
    string methodStr = convertHttpMethodToString(method);

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
    std::thread([this, request]()
    {
        CURL *curl;
        CURLcode res;
        std::string buffer;
        std::string responseHeader;
        curl = curl_easy_init();
        if (curl)
        {
            setupCURLRequest(curl, HttpMethod::GET, request);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeader);
            CURLcode response = curl_easy_perform(curl);
            if(response != CURLE_OK)
            {
                std::cout << "Error: " << curl_easy_strerror(response) << std::endl;
                request.getOnErrorCallback()(responseHeader, buffer);
                throw std::runtime_error("Error: " + std::string(curl_easy_strerror(response)));
            }
            else{
                request.getOnSuccessCallback()(responseHeader, buffer);
            }
        
            std::cout << "Response code: " << response << std::endl;
            curl_easy_cleanup(curl);
        }
        else
        {
            std::cout << "Error initializing curl" << std::endl;
        } 
    })
        .detach();
}

void HttpClient::post(Request request)
{
    std::thread([this, request]()
    {
        CURL *curl;
        CURLcode res;
        std::string buffer;
        std::string responseHeader;
        curl = curl_easy_init();
        if (curl)
        {
            setupCURLRequest(curl, HttpMethod::POST, request);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeader);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.getBody().c_str());
            CURLcode response = curl_easy_perform(curl);
            if(response != CURLE_OK)
            {
                std::cout << "Error: " << curl_easy_strerror(response) << std::endl;
                request.getOnErrorCallback()(responseHeader, buffer);
                throw std::runtime_error("Error: " + std::string(curl_easy_strerror(response)));
            }
            else{
                request.getOnSuccessCallback()(responseHeader, buffer);
            }
        
            std::cout << "Response code: " << response << std::endl;
            curl_easy_cleanup(curl);
        }
        else
        {
            std::cout << "Error initializing curl" << std::endl;
        } 
    }).detach();
}

void HttpClient::put(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl, HttpMethod::PUT, request);
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
        setupCURLRequest(curl, HttpMethod::PATCH, request);
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
        setupCURLRequest(curl, HttpMethod::DELETE, request);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

void HttpClient::head(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl, HttpMethod::HEAD, request);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

void HttpClient::options(Request request)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl, HttpMethod::OPTIONS, request);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}