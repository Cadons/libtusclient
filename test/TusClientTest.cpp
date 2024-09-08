#include <gtest/gtest.h>
#include <iostream>

#include "TusClient.h"
namespace TUS::Test {

    TEST(TusClient, clientCreationTest) {

        TUS::TusClient client("http://localhost:8080/files", "test.txt");

        EXPECT_EQ(client.getUrl(), "http://localhost:8080/files");
        EXPECT_EQ(client.getFilePath(), "test.txt");
        EXPECT_EQ(client.status(), TUS::TusStatus::READY);

    }


}