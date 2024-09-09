/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <thread>
#include <iostream>

#include "http/HttpClient.h"
#include "http/Request.h"

using TUS::Http::HttpClient;

HttpClient::HttpClient()
{
}

HttpClient::~HttpClient()
{
}
size_t write_data(void *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(reinterpret_cast<char *>(ptr), size * nmemb);
    return size * nmemb;
}
std::string HttpClient::convertHttpMethodToString(HttpMethod method)
{
    switch (method)
    {
    case HttpMethod::_GET:
        return "GET";
    case HttpMethod::_POST:
        return "POST";
    case HttpMethod::_PUT:
        return "PUT";
    case HttpMethod::_PATCH:
        return "PATCH";
    case HttpMethod::_DELETE:
        return "DELETE";
    case HttpMethod::_HEAD:
        return "HEAD";
    case HttpMethod::_OPTIONS:
        return "OPTIONS";
    default:
        return "GET";
    }
}

void HttpClient::setupCURLRequest(CURL *curl, HttpMethod method,
                                  Request request)
{
    string methodStr = convertHttpMethodToString(method);

    curl_easy_setopt(curl, CURLOPT_URL, request.getUrl().c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, methodStr.c_str());
    struct curl_slist *headers = NULL;
    for (auto const &header : request.getHeaders())
    {
        headers = curl_slist_append(headers, (header.first + ": " +
                                              header.second)
                                                 .c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

int HttpClient::sendRequest(HttpMethod method, Request request)
{
    int &id = ++m_requestID;
    std::map<int, CURL *> &requests = m_requests;
    std::thread([this, method, request, id, &requests]()
                {
                    
        CURL *curl;
        CURLcode res;
        std::string buffer;
        std::string responseHeader;

        curl = curl_easy_init();
        if (curl) {

            setupCURLRequest(curl, method, request);
             
            requests.insert(std::pair<int, CURL *>(id, curl));

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeader);
            if (method == HttpMethod::_POST|| method == HttpMethod::_PUT|| 
            method == HttpMethod::_PATCH) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.getBody().c_str());
            }
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
            CURLcode response = curl_easy_perform(curl);
            if(response != CURLE_OK)
            {
                std::cout << "Error: " << curl_easy_strerror(response) << std::endl;
                request.getOnErrorCallback()(responseHeader, buffer);
                throw std::runtime_error("Error: " + std::string(curl_easy_strerror(response)));
            }else {
                request.getOnSuccessCallback()(responseHeader, buffer);
            }
        
            std::cout << "Response code: " << response << std::endl;
            curl_easy_cleanup(curl);
        }
        else
        {
            std::cout << "Error initializing curl" << std::endl;
        } })
        .detach();

    return id;
}

int HttpClient::get(Request request)
{
    if (request.getMethod() != HttpMethod::_GET)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_GET, request);
}

int HttpClient::post(Request request)
{
    if (request.getMethod() != HttpMethod::_POST)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_POST, request);
}

int HttpClient::put(Request request)
{
    if (request.getMethod() != HttpMethod::_PUT)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_PUT, request);
}

int HttpClient::patch(Request request)
{
    if (request.getMethod() != HttpMethod::_PATCH)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_PATCH, request);
}

int HttpClient::del(Request request)
{
    if (request.getMethod() != HttpMethod::_DELETE)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_DELETE, request);
}

int HttpClient::head(Request request)
{
    if (request.getMethod() != HttpMethod::_HEAD)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_HEAD, request);
}

int HttpClient::options(Request request)
{
    if (request.getMethod() != HttpMethod::_OPTIONS)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_OPTIONS, request);
}

void HttpClient::abortRequest(int requestID)
{
    std::map<int, CURL *> &requests = m_requests;
    CURL *curl = requests[requestID];
    if (curl)
    {
        curl_easy_cleanup(curl);
        requests.erase(requestID);

        std::cout << "Request " << requestID << " aborted" << std::endl;
    }
}