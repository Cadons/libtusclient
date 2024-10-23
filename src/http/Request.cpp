/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <iostream>

#include "http/Request.h"
using TUS::Http::HttpMethod;
using TUS::Http::Request;


Request::Request()
{
    this->url = "";
    this->body = "";
    this->method = HttpMethod::_GET;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(const Request &request): url(request.url), body(request.body), method(request.method), headers(request.headers), m_onSuccessCallback(request.m_onSuccessCallback), m_onErrorCallback(request.m_onErrorCallback)
{

}

Request::Request(string url)
{
    this->url = url;
    this->body = "";
    this->method = HttpMethod::_GET;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body)
{
    this->url = url;
    this->body = body;
    this->method = HttpMethod::_POST;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method)
{
    this->url = url;
    this->body = body;
    this->method = method;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method, map<string, string> headers)
{
    this->url = url;
    this->body = body;
    this->method = method;
    this->headers = headers;
    if (this->headers.find("Content-Type") == this->headers.end())
    {
        this->headers["Content-Type"] = "application/json";
    }
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method, map<string, string> headers, SuccessCallback onSuccessCallback)
{
    this->url = url;
    this->body = body;
    this->method = method;
    this->headers = headers;
    if (this->headers.find("Content-Type") == this->headers.end())
    {
        this->headers["Content-Type"] = "application/json";
    }
    setOnSuccessCallback(onSuccessCallback);
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method, map<string, string> headers, SuccessCallback onSuccessCallback, ErrorCallback onErrorCallback)
{
    this->url = url;
    this->body = body;
    this->method = method;
    this->headers = headers;
    if (this->headers.find("Content-Type") == this->headers.end())
    {
        this->headers["Content-Type"] = "application/json";
    }
    setOnSuccessCallback(onSuccessCallback);
    setOnErrorCallback(onErrorCallback);
}



Request &Request::operator=(const Request &request)
{
    this->url = request.url;
    this->body = request.body;
    this->method = request.method;
    this->headers = request.headers;
    setOnSuccessCallback(request.getOnSuccessCallback());
    setOnErrorCallback(request.getOnErrorCallback());
    return *this;
}

void Request::addHeader(string key, string value)
{
    this->headers[key] = value;
}

void Request::autorizationHeader(string berearToken)
{
    this->headers["Authorization"] = "Bearer " + berearToken;
}

string Request::getUrl() const
{
    return this->url;
}

string Request::getBody() const
{
    return this->body;
}

HttpMethod Request::getMethod() const
{
    return this->method;
}

map<string, string> Request::getHeaders() const
{
    return this->headers;
}

void Request::setOnSuccessCallback(SuccessCallback onSuccessCallback)
{
    this->m_onSuccessCallback = onSuccessCallback;
}

void Request::setOnErrorCallback(ErrorCallback onErrorCallback)
{
    this->m_onErrorCallback = onErrorCallback;
}

Request::SuccessCallback Request::getOnSuccessCallback() const
{
    return this->m_onSuccessCallback;
}

Request::ErrorCallback Request::getOnErrorCallback() const
{
    return this->m_onErrorCallback;
}

Request::SuccessCallback Request::defaultSuccessCallback()
{
    return [](string header, string data)
    {
        std::cout << header << std::endl;
        std::cout << data << std::endl;
        std::cout << "Successful callback not implemented" << std::endl;
    };
}

Request::ErrorCallback Request::defaultErrorCallback()
{
    return [](string header, string data)
    {
        std::cout << "Failed callback not implemented" << std::endl;
    };
}