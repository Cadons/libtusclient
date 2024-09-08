#include "http/HttpClient.h"
#include "http/Request.h"
#include <gtest/gtest.h>
#include <functional>
#include <thread>
namespace TUS::Test::Http {

using namespace TUS::Http;

TEST(HttpClientTest, ConvertHttpMethodToString)
{

    HttpClient httpClient;
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_GET), "GET");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_POST), "POST");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_PUT), "PUT");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_PATCH), "PATCH");
    EXPECT_EQ(httpClient.convertHttpMethodToString(HttpMethod::_DELETE), "DELETE");
}

struct TestParams
{
    std::string url;
    std::string body;
    HttpMethod method;
    std::string expectedData;
};

class HttpClientParameterizedTest : public testing::TestWithParam<TestParams>
{
protected:
    void SetUp() override
    {
        m_httpClient = new HttpClient();
    }

    void TearDown() override
    {
        delete m_httpClient;
    }

    HttpClient* m_httpClient;
    const int m_timeout = 2;//seconds
};

TEST_P(HttpClientParameterizedTest, HttpRequest)
{
    const TestParams& testCase = GetParam();
    std::string finalResult = "";

    std::function<void(std::string, std::string)> onDataReceivedCallback = [&](std::string header, std::string data)
    {
        std::cout << data << std::endl;
        std::cout << header << std::endl;
        finalResult = data;
    };



    Request request(testCase.url, testCase.body, testCase.method);
    request.setOnSuccessCallback(onDataReceivedCallback);

    switch (testCase.method)
    {
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

    std::this_thread::sleep_for(std::chrono::seconds(m_timeout));
    EXPECT_EQ(finalResult, testCase.expectedData);
}

INSTANTIATE_TEST_SUITE_P(HttpClientTest, HttpClientParameterizedTest, testing::Values(
     TestParams{"http://localhost:3000/files", "", HttpMethod::_GET, "{\"test\":\"get passed\"}"},
    TestParams{"http://localhost:3000/files", "{\"data\":[1,2,3,4,5]}", HttpMethod::_POST, "{\"test\":\"post passed\"}"},
    TestParams{"http://localhost:3000/files", "", HttpMethod::_PUT, "{\"test\":\"put passed\"}"},
    TestParams{"http://localhost:3000/files", "", HttpMethod::_PATCH, "{\"test\":\"patch passed\"}"},
    TestParams{"http://localhost:3000/files", "", HttpMethod::_DELETE, "{\"test\":\"delete passed\"}"}
), [](const testing::TestParamInfo<HttpClientParameterizedTest::ParamType>& info) {
    return  HttpClient::convertHttpMethodToString(info.param.method);
});

} // namespace TUS::Test::Http