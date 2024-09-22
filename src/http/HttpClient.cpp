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
using TUS::Http::HttpMethod;
using TUS::Http::IHttpClient;
using TUS::Http::Request;

HttpClient::HttpClient()
{
}

HttpClient::~HttpClient()
{
}
size_t HttpClient::writeDataCallback(void *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(reinterpret_cast<char *>(ptr), size * nmemb);
    return size * nmemb;
}
int HttpClient::progressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{

    return 0;
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
    Progress progress;
    if (request.getUrl().find("https://") == 0)
    {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    list<string> badURLChars = {"<", ">", "#", "%", "{", "}", "|", "\\", "^", "~", "[", "]", "`"};
    for (auto const &badChar : badURLChars)
    {
        if (request.getUrl().back() == badChar[0])
        {
            std::string requestSummury = request.getUrl() + " " + methodStr;
            throw std::runtime_error("Bad character (" + badChar + ")in URL\n" + requestSummury);
        }
    }
    curl_easy_setopt(curl, CURLOPT_URL, request.getUrl().c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, methodStr.c_str());
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallback);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L); // timeout in seconds for the connection, if the connection is not established in 10 seconds the request will be aborted

    struct curl_slist *headers = NULL;
    for (auto const &header : request.getHeaders())
    {

        headers = curl_slist_append(headers, (header.first + ": " +
                                              header.second)
                                                 .c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

IHttpClient *HttpClient::sendRequest(HttpMethod method, Request request)
{

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        setupCURLRequest(curl, method, request);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeDataCallback);
        switch (method)
        {
        case HttpMethod::_HEAD:
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
            break;
        case HttpMethod::_POST:
        case HttpMethod::_PUT:
        case HttpMethod::_PATCH:
        {
            if (request.getBody().empty())
            {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
            }
            else
            {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.getBody().c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.getBody().size());//set the size of the data
            }
            break;
        }
        default:
            break;
        }
        RequestTask requestTask(request, curl);
        m_requestsQueue.push(requestTask);
    }
    return (IHttpClient *)this;
}

IHttpClient *HttpClient::get(Request request)
{
    if (request.getMethod() != HttpMethod::_GET)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_GET, request);
}

IHttpClient *HttpClient::post(Request request)
{
    if (request.getMethod() != HttpMethod::_POST)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_POST, request);
}

IHttpClient *HttpClient::put(Request request)
{
    if (request.getMethod() != HttpMethod::_PUT)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_PUT, request);
}

IHttpClient *HttpClient::patch(Request request)
{
    if (request.getMethod() != HttpMethod::_PATCH)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_PATCH, request);
}

IHttpClient *HttpClient::del(Request request)
{
    if (request.getMethod() != HttpMethod::_DELETE)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_DELETE, request);
}

IHttpClient *HttpClient::head(Request request)
{
    if (request.getMethod() != HttpMethod::_HEAD)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_HEAD, request);
}

IHttpClient *HttpClient::options(Request request)
{
    if (request.getMethod() != HttpMethod::_OPTIONS)
    {
        throw std::runtime_error("Method not allowed");
    }
    return sendRequest(HttpMethod::_OPTIONS, request);
}

IHttpClient *HttpClient::abortAll()
{
    while (!m_requestsQueue.empty())
    {
        m_requestsQueue.pop();
    }
    m_abort = true;
    return (IHttpClient *)this;
}

IHttpClient *HttpClient::execute()
{

        while (!m_requestsQueue.empty())
        {
            if(m_abort)
            {
                m_requestsQueue.pop();
                m_abort = false;
                return (IHttpClient *)this;
            }

            CURL *curl = m_requestsQueue.front().curl;
            string url = m_requestsQueue.front().getUrl();

            string responseHeader;
            string buffer;
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeader);
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
                m_requestsQueue.front().getOnErrorCallback()(responseHeader, buffer);
                return (IHttpClient *)this;
            }
            else
            {

                m_requestsQueue.front().getOnSuccessCallback()(responseHeader, buffer);
            }
            curl_easy_cleanup(curl); // cleanup the curl handle
            m_requestsQueue.pop();
        }

    return (IHttpClient *)this;
}

