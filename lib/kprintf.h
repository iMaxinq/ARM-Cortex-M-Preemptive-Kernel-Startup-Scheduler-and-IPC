#ifndef KPRINTF_H
#define KPRINTF_H

// function pointer type for outputting a single character
typedef void (*custom_kputc_function)(char c);

// register the character output function (e.g., uart_putc on target, buffer writer in unit tests)
void kprintf_init(custom_kputc_function putc_fn);

// custom printf supporting: %c, %s, %d, %u, %x, %p, and %% 
void kprintf(const char *fmt, ...);

#endif /* KPRINTF_H */