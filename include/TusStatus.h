#ifndef TUSSTATUS_H
#define TUSSTATUS_H
#include "libtusclient.h"
namespace TUS
{

    enum class LIBTUSAPI_EXPORT TusStatus
    {
        READY,
        UPLOADING,
        FAILED,
        FINISHED
    };

}
#endif // TUSSTATUS_H