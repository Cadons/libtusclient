/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include "chunk/utility/ChunkUtility.h"
#define KB 1000

using TUS::Chunk::Utility::ChunkUtility;

std::int64_t ChunkUtility::getChunkSizeFromGB(int size)
{
      return static_cast<std::int64_t>( size * KB * KB * KB);
}

std::int64_t ChunkUtility::getChunkSizeFromMB(int size)
{
      return static_cast<std::int64_t>( size * KB * KB);
}

std::int64_t ChunkUtility::getChunkSizeFromKB(int size)
{
      return static_cast<std::int64_t>( size * KB);
}
