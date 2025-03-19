/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include <iostream>

#include "http/Request.h"
using TUS::Http::HttpMethod;
using TUS::Http::Request;


Request::Request() {
    this->url = "";
    this->body = "";
    this->method = HttpMethod::_GET;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(const Request &request): url(request.url), body(request.body), method(request.method),
                                          headers(request.headers), m_onSuccessCallback(request.m_onSuccessCallback),
                                          m_onErrorCallback(request.m_onErrorCallback) {
}

Request::Request(string url) {
    this->url = std::move(url);
    this->body = "";
    this->method = HttpMethod::_GET;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body) {
    this->url = std::move(url);
    this->body = std::move(body);
    this->method = HttpMethod::_POST;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method) {
    this->url = std::move(url);
    this->body = std::move(body);
    this->method = method;
    this->headers = map<string, string>();
    this->headers["Content-Type"] = "application/json";
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method, map<string, string> headers) {
    this->url = std::move(url);
    this->body = std::move(body);
    this->method = method;
    this->headers = std::move(headers);
    if (!this->headers.contains("Content-Type")) {
        this->headers["Content-Type"] = "application/json";
    }
    setOnSuccessCallback(defaultSuccessCallback());
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method, map<string, string> headers,
                 SuccessCallback onSuccessCallback) {
    this->url = std::move(url);
    this->body = std::move(body);
    this->method = method;
    this->headers = std::move(headers);
    if (!this->headers.contains("Content-Type")) {
        this->headers["Content-Type"] = "application/json";
    }
    setOnSuccessCallback(std::move(onSuccessCallback));
    setOnErrorCallback(defaultErrorCallback());
}

Request::Request(string url, string body, HttpMethod method, map<string, string> headers,
                 SuccessCallback onSuccessCallback, ErrorCallback onErrorCallback) {
    this->url = std::move(url);
    this->body = std::move(body);
    this->method = method;
    this->headers = std::move(headers);
    if (!this->headers.contains("Content-Type")) {
        this->headers["Content-Type"] = "application/json";
    }
    setOnSuccessCallback(std::move(onSuccessCallback));
    setOnErrorCallback(std::move(onErrorCallback));
}


Request &Request::operator=(const Request &request) {
    this->url = request.url;
    this->body = request.body;
    this->method = request.method;
    this->headers = request.headers;
    setOnSuccessCallback(request.getOnSuccessCallback());
    setOnErrorCallback(request.getOnErrorCallback());
    return *this;
}

void Request::addHeader(const string &key, const string &value) {
    this->headers[key] = value;
}

void Request::autorizationHeader(const string &berearToken) {
    this->headers["Authorization"] = "Bearer " + berearToken;
}

string Request::getUrl() const {
    return this->url;
}

string Request::getBody() const {
    return this->body;
}

HttpMethod Request::getMethod() const {
    return this->method;
}

map<string, string> Request::getHeaders() const {
    return this->headers;
}

void Request::setOnSuccessCallback(SuccessCallback onSuccessCallback) {
    this->m_onSuccessCallback = std::move(onSuccessCallback);
}

void Request::setOnErrorCallback(ErrorCallback onErrorCallback) {
    this->m_onErrorCallback = std::move(onErrorCallback);
}

Request::SuccessCallback Request::getOnSuccessCallback() const {
    return this->m_onSuccessCallback;
}

Request::ErrorCallback Request::getOnErrorCallback() const {
    return this->m_onErrorCallback;
}

Request::SuccessCallback Request::defaultSuccessCallback() {
    return [](const string &header, const string &data) {
        std::cout << header << std::endl;
        std::cout << data << std::endl;
        std::cout << "Successful callback not implemented" << std::endl;
    };
}

Request::ErrorCallback Request::defaultErrorCallback() {
    return [](const string &header, const string &data) {
        std::cout << "Failed callback not implemented" << std::endl;
    };
}
