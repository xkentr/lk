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

#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <lktypes.h>
#include <dev/uart.h>
#include <target/debugconfig.h>

#undef errno
extern int errno;

// Platform specific filenames and fds.
static const char UART1_FNAME[] = "/dev/uart1";
static const char UART2_FNAME[] = "/dev/uart2";
static const char UART3_FNAME[] = "/dev/uart3";
#define UART_FD_BASE 10
#define UART1_FD (UART_FD_BASE+1)
#define UART2_FD (UART_FD_BASE+2)
#define UART3_FD (UART_FD_BASE+3)

int _stat(const char *filepath, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _open (const char *name, int flags, int mode)
{
	if (!strncmp(name, UART1_FNAME, sizeof(UART1_FNAME)))
		return UART1_FD;
	else if (!strncmp(name, UART2_FNAME, sizeof(UART2_FNAME)))
		return UART2_FD;
	else if (!strncmp(name, UART3_FNAME, sizeof(UART3_FNAME)))
		return UART3_FD;

	errno = ENOSYS;
	return -1;
}

int _close(int file) {
	return -1;
}

static void read_uart(int port, char *ptr, int len)
{
	int n;
	for (n = 0; n < len; n++)
		*ptr++ = uart_getc(port, true);

}

int _read(int file, char *ptr, int len) {
	// Remap stdin/stdout
	if (file == STDIN_FILENO)
		file = UART_FD_BASE + DEBUG_UART;

	switch (file) {
	case UART1_FD:
		read_uart(1, ptr, len);
		break;
	case UART2_FD:
		read_uart(2, ptr, len);
		break;
	case UART3_FD:
		read_uart(3, ptr, len);
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return len;
}

static void write_uart(int port, char *ptr, int len, bool is_stdout)
{
	int n;
	for (n = 0; n < len; n++) {
		if (*ptr == '\n')
			uart_putc(port, '\r');

		uart_putc(port, *ptr++);
	}

}

int _write(int file, char *ptr, int len) {
	bool is_stdout = false;
	// Remap stdin/stdout
	if (file == STDOUT_FILENO ||
		file == STDERR_FILENO) {
		file = UART_FD_BASE + DEBUG_UART;
		is_stdout = true;
	}

    switch (file) {
	case UART1_FD:
		write_uart(1, ptr, len, is_stdout);
		break;
	case UART2_FD:
		write_uart(2, ptr, len, is_stdout);
		break;
	case UART3_FD:
		write_uart(3, ptr, len, is_stdout);
		break;
    default:
        errno = EBADF;
        return -1;
    }

    return len;
}

int _isatty(int file) {
    switch (file){
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }

}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}
