/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */

#include "utility/ChunkUtility.h"

using TUS::Utility::ChunkUtility;

int ChunkUtility::getChunkSizeFromGB(int size)
{
    return size * 1024 * 1024 * 1024;
}

int ChunkUtility::getChunkSizeFromMB(int size)
{
    return size * 1024 * 1024;
}

int ChunkUtility::getChunkSizeFromKB(int size)
{
    return size * 1024;
}
