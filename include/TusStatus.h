#ifndef INCLUDE_TUSSTATUS_H_
#define INCLUDE_TUSSTATUS_H_
/*
 * Copyright (c) 2024 Matteo Cadoni
 * This file is part of libtusclient, licensed under the MIT License.
 * See the LICENSE file in the project root for more information.
 */
#include "libtusclient.h"
namespace TUS
{

    enum class LIBTUSAPI_EXPORT TusStatus
    {
        READY,
        UPLOADING,
        FAILED,
        FINISHED,
        PAUSED
    };

}
#endif // INCLUDE_TUSSTATUS_H_