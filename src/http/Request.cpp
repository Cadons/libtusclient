#include "http/Request.h"

using namespace TUS::Http;

Request::Request()
{
    this->url = "";
    this->body = "";
    this->method = HttpMethod::GET;
}

Request::Request(string url)
{
    this->url = url;
    this->body = "";
    this->method = HttpMethod::GET;
}

Request::Request(string url, string body)
{
    this->url = url;
    this->body = body;
    this->method = HttpMethod::POST;
}

Request::Request(string url, string body, HttpMethod method)
{
    this->url = url;
    this->body = body;
    this->method = method;
}

Request::Request(string url, string body, HttpMethod method, map<string, string> headers)
{
    this->url = url;
    this->body = body;
    this->method = method;
    this->headers = headers;
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
