#include <string.h>
#include <stdarg.h>
#include "rprint.h"

#define BUFFER_SIZE 1024

static char buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

size_t str_len(const char *str) {
	const char *s = str;
	while (*s) s++;
	return s - str;
}

void sys_write(const char *str, size_t length) {
	//x86-64 Linux syscall
	
	__asm__ volatile (
	"mov $1, %%rax\n" // syscall
	"mov $1, %%rdi\n" // fd
	"mov %0, %%rsi\n" // pointer
	"mov %1, %%rdx\n" // length
	"syscall"
	:
	: "r" (str), "r" (length)
	: "rax", "rdi", "rsi", "rdx", "rcx", "r11", "memory");
}

size_t int_to_str(int num, char *buf) {
	char *p = buf;
	int is_negative = 0;

	if (num < 0) {
		is_negative = 1;
		num = -num;
	}

	do {
		*p++ = '0' + (num % 10);
		num /= 10;
	} while (num > 0);

	if (is_negative) { *p++ = '-'; }

	size_t len = p - buf;
	for (size_t i = 0; i < len / 2; i++) {
		char temp = buf[i];
		buf[i] = buf[len - i - 1];
		buf[len - i - 1] = temp;
	}

	*p = '\0';
	return len;
}

void flush_buffer() {
	if (buffer_index > 0) {
		sys_write(buffer, buffer_index);
		buffer_index = 0;
	}
}

void buffered_write(const char *str, size_t len) {

	if (len >= BUFFER_SIZE) {
		flush_buffer();
		sys_write(str, len);
		return;
	}

	if (buffer_index + len >= BUFFER_SIZE) {
		flush_buffer();
	}


	for (size_t i = 0; i < len; i++) {
		buffer[buffer_index++] = str[i];
	}
}

void rprint(const char *format, ...) {
	va_list args;
	va_start(args, format);

	char num_buf[32];
	size_t i = 0;

	while (format[i] != '\0') {
		if (format[i] == '%' && format[i+1] != '\0') {
		i++;

		switch (format[i]) {
		case 'd': {
			int num = va_arg(args, int);
			size_t len = int_to_str(num, num_buf);
			buffered_write(num_buf, len);
			break;
			  }
		case 's': {
			char *str = va_arg(args, char*);
			size_t len = str_len(str);
			buffered_write(str, len);
			break;
			  }
		case 'c': {
			char c = (char)va_arg(args, int);
			buffered_write(&c, 1);
			break;
			  }
		case '%': {
			buffered_write("%", 1);
			break;
			  }
		default: {
			buffered_write("%", 1);
			buffered_write(&format[i], 1);
			break;
			 }
			}
		} else {
			buffered_write(&format[i], 1);
		}
		i++;
	}	

	va_end(args);
	flush_buffer();
}
