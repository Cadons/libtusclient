/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <gtest/gtest.h>
#include "verifiers/IFileVerifier.h"
#include "verifiers/Md5Verifier.h"

using TUS::FileVerifier::Md5Verifier;

class FileVerifierTest : public ::testing::TestWithParam<std::tuple<std::shared_ptr<TUS::FileVerifier::IFileVerifier>, std::vector<uint8_t>, std::string>> {};

TEST_P(FileVerifierTest, VerifyHash)
{
    auto param = GetParam();
    auto verifier = std::get<0>(param);
    std::vector<uint8_t> buffer = std::get<1>(param);
    std::string expected_hash = std::get<2>(param);
    std::string hash = verifier->hash(buffer);
    ASSERT_EQ(hash, expected_hash);
    ASSERT_TRUE(verifier->verify(buffer, hash));
}

INSTANTIATE_TEST_SUITE_P(
    FileVerifierTests,
    FileVerifierTest,
    ::testing::Values(
        std::make_tuple(std::make_shared<Md5Verifier>(), std::vector<uint8_t>{'a', 'b', 'c'}, "900150983cd24fb0d6963f7d28e17f72")));
