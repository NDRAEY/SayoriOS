#pragma once

#include <common.h>
#include "io/screen.h"
#include <stdarg.h>

typedef enum {
    TTY_ATTRIBUTE_COLOR = (1 << 31)
} tty_attribute_type_t;

void tty_fontConfigurate();

void tty_init();

void tty_reset_position();
void tty_clear_buffers();
void tty_build_buffers();

void tty_set_color(uint32_t color);
uint32_t tty_get_color();

void _tty_putc(int c);
void tty_putc(int c);
void _tty_puts(const char* str);
void tty_puts(const char *str);
void _tty_vprintf(const char* format, va_list args);
void tty_vprintf(const char* format, va_list args);

void tty_set_x(uint32_t x);
void tty_set_y(uint32_t y);

void tty_render();
void tty_scroll();

void tty_clear();

static inline void tty_printf(char *text, ...) {
    va_list args;
    va_start(args, text);
    tty_vprintf(text, args);
    va_end(args);
}