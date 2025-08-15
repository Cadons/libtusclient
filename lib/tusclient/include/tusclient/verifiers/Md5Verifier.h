/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#ifndef INCLUDE_VERIFIERS_MD5VERIFIER_H_
#define INCLUDE_VERIFIERS_MD5VERIFIER_H_

#include "IFileVerifier.h"
#include "libtusclient.h"

namespace TUS::FileVerifier{
        class EXPORT_LIBTUSCLIENT Md5Verifier : public IFileVerifier
        {
        public:
            Md5Verifier();
            ~Md5Verifier() override;
            [[nodiscard]] string hash(const std::vector<uint8_t> &buffer) const override;
            [[nodiscard]] bool verify(const std::vector<uint8_t> &buffer, const string &hash) const override;
        };
    } // namespace Verifiers



#endif // INCLUDE_VERIFIERS_MD5VERIFIER_H_