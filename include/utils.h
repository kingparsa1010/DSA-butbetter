/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#ifndef DSA_UTILS_H
#define DSA_UTILS_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

typedef char BYTE;
typedef BYTE *PBYTE;

typedef enum _STATUS
{
    SUCCESS = 0,
    ERROR = 1,
    FILE_NOT_FOUND = 2,
    INVALIDARG = 3,
    ACCESSDENIED = 13,
    OUTOFMEMORY = 14,
    HANDLE = 38,
    POINTER = 125,
    NOTIMPL = 213,
    NOINIT = 214,
    BOUNDS = 280,
    STACK_UNDERFLOW = 299,
    STACK_OVERFLOW = 300,
    ILLEGAL_METHOD_CALL = 310,
    ILLEGAL_DELEGATE_ASSIGNMENT = 311,
    ILLEGAL_STATE_CHANGE = 332,
} STATUS;

#define FAILED(st) ((STATUS)(st) > 0)
#define RETURN_IF_FAILED(st) do { STATUS stRet_ = (st); if (FAILED(stRet_)) { return stRet_; }} while (0)


#ifdef __cplusplus

#include <stdexcept>
#include <string>
#include <iostream>

struct Exception final : std::runtime_error
{
    STATUS status;
    std::string msg;

    explicit Exception( STATUS s ): std::runtime_error( "Unhandled Exception: " + std::to_string(s) ), status(s)
    {
        //std::printf( "Exception %d within C++ code.\n", status );
    }

    explicit Exception( STATUS s, const std::string &message ): std::runtime_error( "Unhandled Exception: " + std::to_string(s) + " with message " + message ), status(s), msg(message)
    {
        //std::printf( "Exception %d within C++ code with message \"%s\".\n", status, message.c_str() );
    }
};

#define check_st_( st ) do { STATUS status_ = (st); if ( FAILED( status_ ) ) throw Exception( status_ ); } while (0)

#endif // __cplusplus

#endif // DSA_UTILS_H
