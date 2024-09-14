/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <gtest/gtest.h>
#include <iostream>

#include "TusClient.h"
namespace TUS::Test {

    TEST(TusClient, clientCreationTest) {

        TUS::TusClient client("http://localhost:3000/files", "test.txt");

        EXPECT_EQ(client.getUrl(), "http://localhost:8080/files");
        EXPECT_EQ(client.getFilePath(), "test.txt");
        EXPECT_EQ(client.status(), TUS::TusStatus::READY);

    }

    TEST(TusClient, uploadTest) {

        TUS::TusClient client("http://localhost:3000/files", "test.txt");

        client.upload();


    }
    


}