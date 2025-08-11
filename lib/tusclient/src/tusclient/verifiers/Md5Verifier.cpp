/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include <sstream>
#include <vector>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iomanip>

#include "verifiers/Md5Verifier.h"


using TUS::FileVerifier::Md5Verifier;

Md5Verifier::Md5Verifier()
= default;

Md5Verifier::~Md5Verifier()
= default;

string Md5Verifier::hash(const std::vector<uint8_t> &buffer) const {
    boost::uuids::detail::md5 hash;
    hash.process_bytes(buffer.data(), buffer.size());
    boost::uuids::detail::md5::digest_type digest;
    hash.get_digest(digest);

    std::string result;
    for (const unsigned char i : digest) {
        result += std::format("{:02x}", static_cast<int>(i));
    }
    return result;
}

bool Md5Verifier::verify(const std::vector<uint8_t> &buffer, const string &hash) const {
    return this->hash(buffer) == hash;
}
