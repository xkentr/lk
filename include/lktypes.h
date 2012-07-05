/*
 * Copyright (c) 2012 Kent Ryhorchuk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __LKTYPES_H
#define __LKTYPES_H

#include <stdint.h>
#include <limits.h>

#ifndef __cplusplus
typedef int bool;
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0
#endif

typedef unsigned long long bigtime_t;
#define INFINITE_TIME ULONG_MAX

typedef int status_t;

typedef uintptr_t addr_t;
typedef uintptr_t vaddr_t;
typedef uintptr_t paddr_t;

enum handler_return {
	INT_NO_RESCHEDULE = 0,
	INT_RESCHEDULE,
};

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#define TIME_GTE(a, b) ((long)((a) - (b)) >= 0)
#define TIME_LTE(a, b) ((long)((a) - (b)) <= 0)
#define TIME_GT(a, b) ((long)((a) - (b)) > 0)
#define TIME_LT(a, b) ((long)((a) - (b)) < 0)

#endif
