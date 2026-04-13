    #pragma once

    typedef enum status {
        SUCCESS = 0,
        NULL_POINTER,
        ALLOC_FAIL,
        UNDERFLOW,
        OUT_OF_RANGE,
        NOT_FOUND,
        DATABASE_ERROR
    } status;