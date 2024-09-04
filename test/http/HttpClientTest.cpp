#include "http/HttpClient.h"
#include "http/Request.h"
#include <gtest/gtest.h>

using namespace TUS::Http;

TEST(HttpClientTest, ConvertHttpMethodToString)
{
    HttpClient httpClient;
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::GET), "GET");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::POST), "POST");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::PUT), "PUT");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::PATCH), "PATCH");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::DELETE), "DELETE");
}


TEST(HttpClientTest, Get)
{
    HttpClient httpClient;
    Request request("https://localhost:3000/", "", HttpMethod::GET);
    httpClient.get(request);
    // Add assertions to validate the GET request
}

TEST(HttpClientTest, Post)
{
    HttpClient httpClient;
    Request request("https://localhost:3000", "", HttpMethod::POST);
    httpClient.post(request);
    // Add assertions to validate the POST request
}

TEST(HttpClientTest, Put)
{
    HttpClient httpClient;
    Request request("https://localhost:3000", "", HttpMethod::PUT);
    httpClient.put(request);
    // Add assertions to validate the PUT request
}

TEST(HttpClientTest, Patch)
{
    HttpClient httpClient;
    Request request("https://localhost:3000", "", HttpMethod::PATCH);
    httpClient.patch(request);
    // Add assertions to validate the PATCH request
}

TEST(HttpClientTest, Delete)
{
    HttpClient httpClient;
    Request request("https://localhost:3000", "", HttpMethod::DELETE);
    httpClient.del(request);
    // Add assertions to validate the DELETE request
}