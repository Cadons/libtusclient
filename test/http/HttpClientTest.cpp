/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <thread>
#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include "http/HttpClient.h"
#include "http/Request.h"

namespace TUS::Test::Http {
    using TUS::Http::HttpClient;
    using TUS::Http::HttpMethod;
    using TUS::Http::Request;

    TEST(HttpClientTest, ConvertHttpMethodToString) {
        HttpClient httpClient;
        EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_GET), "GET");
        EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_POST), "POST");
        EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_PUT), "PUT");
        EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_PATCH), "PATCH");
        EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_DELETE), "DELETE");
    }

    struct TestParams {
        std::string url;
        std::string body;
        HttpMethod method;
        std::string expectedData;
    };

    class HttpClientParameterizedTest : public testing::TestWithParam<TestParams> {
    protected:
        void SetUp() override {
            m_httpClient = new HttpClient();
        }

        void TearDown() override {
            delete m_httpClient;
        }

        HttpClient *m_httpClient{};
        const int m_timeout = 2; //seconds
    };

    TEST_P(HttpClientParameterizedTest, HttpRequest) {
        const TestParams &testCase = GetParam();
        std::string finalResult;

        std::function<void(std::string, std::string)> onDataReceivedCallback = [&
                ](const std::string &header, const std::string &data) {
            std::cout << data << std::endl;
            std::cout << header << std::endl;
            finalResult = data;
        };


        Request request(testCase.url, testCase.body, testCase.method);
        request.setOnSuccessCallback(onDataReceivedCallback);

        switch (testCase.method) {
            case HttpMethod::_GET:
                m_httpClient->get(request);
                break;
            case HttpMethod::_POST:
                m_httpClient->post(request);
                break;
            case HttpMethod::_PUT:
                m_httpClient->put(request);
                break;
            case HttpMethod::_PATCH:
                m_httpClient->patch(request);
                break;
            case HttpMethod::_DELETE:
                m_httpClient->del(request);
                break;
            case HttpMethod::_HEAD:
                m_httpClient->head(request);
                break;
            case HttpMethod::_OPTIONS:
                m_httpClient->options(request);
                break;
        }
        m_httpClient->execute();
        EXPECT_EQ(finalResult, testCase.expectedData);
    }

    INSTANTIATE_TEST_SUITE_P(HttpClientTest, HttpClientParameterizedTest, testing::Values(
                                 TestParams{"http://localhost:3000/files", "", HttpMethod::_GET,
                                 "{\"test\":\"get passed\"}"},
                                 TestParams{"http://localhost:3000/files", "{\"data\":[1,2,3,4,5]}", HttpMethod::_POST,
                                 "{\"test\":\"post passed\"}"},
                                 TestParams{"http://localhost:3000/files", "", HttpMethod::_PUT,
                                 "{\"test\":\"put passed\"}"},
                                 TestParams{"http://localhost:3000/files", "", HttpMethod::_PATCH,
                                 "{\"test\":\"patch passed\"}"},
                                 TestParams{"http://localhost:3000/files", "", HttpMethod::_DELETE,
                                 "{\"test\":\"delete passed\"}"},
                                 TestParams{"http://localhost:3000/files", "", HttpMethod::_HEAD, ""}
                             ), [](const testing::TestParamInfo<HttpClientParameterizedTest::ParamType>& info) {
                             return HttpClient::convertHttpMethodToString(info.param.method);
                             });

    TEST_F(HttpClientParameterizedTest, CheckWrongMethod) {
        Request request("http://localhost:3000/files", "", HttpMethod::_GET);
        EXPECT_THROW(m_httpClient->put(request), std::runtime_error);
    }

    TEST_F(HttpClientParameterizedTest, AbortAll) {
        Request request("http://localhost:3000/files", "", HttpMethod::_GET);
        Request request2("http://localhost:3000/files", "", HttpMethod::_GET);
        m_httpClient->get(request);
        m_httpClient->get(request2);
        m_httpClient->abortAll();
        m_httpClient->execute();
    }

    TEST_F(HttpClientParameterizedTest, HttpsRequest) {
        Request request("https://www.google.com", "", HttpMethod::_GET);
        request.setOnSuccessCallback([this](const std::string &header,
                                            const std::string &data) {
            std::cout << data << std::endl;
            std::cout << header << std::endl;

            ASSERT_TRUE(!data.empty());
        });
        request.setOnErrorCallback([this](const std::string &header,
                                          const std::string &data) {
            ASSERT_TRUE(false);
        });
        m_httpClient->get(request);
        m_httpClient->execute();
    }

    TEST_F(HttpClientParameterizedTest, AuthorizedRequestSuccess) {
        Request request("http://localhost:3000/auth/files", "", HttpMethod::_POST);
        request.setOnSuccessCallback([this](const std::string &header,
                                            const std::string &data) {
            ASSERT_EQ(HttpClient::getHttpReturnCode(header), 200);
        });
        request.setOnErrorCallback([this](const std::string &header,
                                          const std::string &data) {
            FAIL();
        });
        m_httpClient->setAuthorization(
            "WAmjPKLlw6287Ky9L8mVSvI0cwEu5gvd1Y0cj9uPioLlR0E6CJqmExeJOgaO6YV5YECMctqSYQPRR6eC9p3hag5rkBdMjA7Z6Y6zQfDofepIuOwdZ820qDpfljB");
        m_httpClient->post(request);
        m_httpClient->execute();
    }

    TEST_F(HttpClientParameterizedTest, AuthorizedRequestFail) {
        Request request("http://localhost:3000/auth/files", "", HttpMethod::_POST);
        request.setOnSuccessCallback([this](const std::string &header,
                                            const std::string &data) {
            FAIL();
        });
        request.setOnErrorCallback([this](const std::string &header,
                                          const std::string &data) {
            ASSERT_EQ(HttpClient::getHttpReturnCode(header), 401);
        });
        m_httpClient->setAuthorization("wrongToken");
        m_httpClient->post(request);
        m_httpClient->execute();
    }
} // namespace TUS::Test::Http
