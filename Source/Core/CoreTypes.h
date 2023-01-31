#pragma once

// Typedefs
#include <assert.h>
#include <initializer_list>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Standard integers
// Unsigned
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Floating points
typedef float f32;
typedef double f64;

// Memory model
typedef size_t usize;
typedef uintptr_t uptr;
typedef intptr_t iptr;

// Sizes
#define Kilobytes(value) ((value) *1024LL)
#define Megabytes(value) (Kilobytes(value) * 1024LL)
#define Gigabytes(value) (Megabytes(value) * 1024LL)
#define Terabytes(value) (Gigabytes(value) * 1024LL)

// Pointers
#define AlignPow2(value, alignment) (((value) + ((alignment) -1)) & ~(((value) - (value)) + (alignment) -1))
#define Align4(value) (((value) + 3) & ~3)
#define Align8(value) (((value) + 7) & ~7)
#define Align16(value) (((value) + 15) & ~15)