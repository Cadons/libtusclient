/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include "chunk/TUSChunk.h"

using TUS::Chunk::TUSChunk;

TUSChunk::TUSChunk(std::vector<uint8_t> data, size_t offset)
    : m_data(data), m_chunkSize(offset)
{
}

std::vector<uint8_t> TUSChunk::getData() const
{
    return m_data;
}

size_t TUSChunk::getChunkSize() const
{
    return m_chunkSize;
}



