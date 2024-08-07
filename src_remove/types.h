/**
 * @file types.h
 * @brief Defines common types.
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#pragma once

#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
#define _QWORD __int64
#define _DWORD __int32
#define _WORD __int16
#define _BYTE char

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef u32 uint;
typedef char16_t wchar;

// log functions
#ifdef DEBUG
#define debug(mes) botwsavs::fs::Logger::Instance().Debug(mes)
#define debugf(mes, value) botwsavs::fs::Logger::Instance().DebugF(mes, value)
#else
#define debug(mes) 
#define debugf(mes)
#endif
#define info(mes) botwsavs::fs::Logger::Instance().Info(mes)
#define warn(mes) botwsavs::fs::Logger::Instance().Warn(mes)
#define error(mes) botwsavs::fs::Logger::Instance().Error(mes)
#define infof(mes, value) botwsavs::fs::Logger::Instance().InfoF(mes, value)
#define warnf(mes, value) botwsavs::fs::Logger::Instance().WarnF(mes, value)
#define errorf(mes, value) botwsavs::fs::Logger::Instance().ErrorF(mes, value)

#define nameof(x) #x
#define named(x) #x, x
