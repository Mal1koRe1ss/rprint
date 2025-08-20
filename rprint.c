#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "rprint.h"

#define BUFFER_SIZE 8192  

static char buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

size_t str_len(const char *str) {
    const char *s = str;
    while (((uintptr_t)s & 7) && *s) s++;
    
    if (*s) {
        const uint64_t *word_ptr = (const uint64_t *)s;
        uint64_t word;
        
        while (1) {
            word = *word_ptr++;
            if ((word - 0x0101010101010101ULL) & ~word & 0x8080808080808080ULL) {
                s = (const char *)(word_ptr - 1);
                while (*s) s++;
                break;
            }
        }
    }
    
    return s - str;
}

static inline void sys_write(const char *str, size_t length) {
    register long rax __asm__("rax") = 1;        // SYS_write
    register long rdi __asm__("rdi") = 1;        // stdout
    register const char *rsi __asm__("rsi") = str;
    register size_t rdx __asm__("rdx") = length;
    
    __asm__ volatile (
        "syscall"
        : "+r" (rax)
        : "r" (rdi), "r" (rsi), "r" (rdx)
        : "rcx", "r11", "memory"
    );
}

size_t int_to_str(int num, char *buf) {
    char *p = buf;
    
    if (num == 0) {
        *p++ = '0';
        *p = '\0';
        return 1;
    }
    
    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    char temp[12];
    char *temp_p = temp;
    
    while (num > 0) {
        *temp_p++ = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        *p++ = '-';
    }

    while (temp_p > temp) {
        *p++ = *--temp_p;
    }

    *p = '\0';
    return p - buf;
}

static inline void flush_buffer() {
    if (buffer_index > 0) {
        sys_write(buffer, buffer_index);
        buffer_index = 0;
    }
}

static inline void buffered_write(const char *str, size_t len) {
    if (len >= BUFFER_SIZE) {
        flush_buffer();
        sys_write(str, len);
        return;
    }

    if (buffer_index + len >= BUFFER_SIZE) {
        flush_buffer();
    }

    char *dest = buffer + buffer_index;
    const char *src = str;
    
    while (len >= 8) {
        *(uint64_t*)dest = *(const uint64_t*)src;
        dest += 8;
        src += 8;
        len -= 8;
    }
    
    while (len--) {
        *dest++ = *src++;
    }
    
    buffer_index += (dest - (buffer + buffer_index));
}

static inline void buffered_write_char(char c) {
    if (buffer_index >= BUFFER_SIZE) {
        flush_buffer();
    }
    buffer[buffer_index++] = c;
}

void rprint(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char num_buf[32];
    const char *fmt = format;
    const char *start = format;
    
    while (*fmt) {
        if (*fmt == '%' && fmt[1]) {
            if (fmt > start) {
                buffered_write(start, fmt - start);
            }
            
            fmt++; // skip '%'
            
            switch (*fmt) {
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
                buffered_write_char(c);
                break;
            }
            case '%': {
                buffered_write_char('%');
                break;
            }
            default: {
                buffered_write_char('%');
                buffered_write_char(*fmt);
                break;
            }
            }
            
            fmt++;
            start = fmt; 
        } else {
            fmt++;
        }
    }
    
    if (fmt > start) {
        buffered_write(start, fmt - start);
    }

    va_end(args);
    flush_buffer();
}
