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

/* Newlib stubs that can be defined at the kernel level. */

#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include <kernel/thread.h>

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);
extern unsigned int _end;
extern unsigned int _end_of_ram;
caddr_t heap_end = 0;

#define OOM_STR "Out of memory\n"
caddr_t _sbrk(int incr) {
    caddr_t prev_heap_end;

    if (heap_end == 0)
        heap_end = (caddr_t)&_end;

    prev_heap_end = heap_end;
    if (heap_end + incr > (caddr_t)&_end_of_ram) {
        _write (STDERR_FILENO, OOM_STR, sizeof(OOM_STR)-1);
	errno = ENOMEM;
	return  (caddr_t) -1;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

void __malloc_lock(struct _reent *_r)
{
	enter_critical_section();
}


void __malloc_unlock(struct _reent *_r)
{
	exit_critical_section();
}

void _exit(int status) {
    thread_exit(status);
}
