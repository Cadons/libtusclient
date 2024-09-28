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
