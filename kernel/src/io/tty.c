/**
 * @file io/tty.c
 * @author Андрей Павленко (pikachu_andrey@vk.com)
 * @brief Интерфейс для работы с виртуальным терминалом
 * @version 0.3.5
 * @date 2024-05-23
 * @copyright Copyright SayoriOS Team (c) 2022-2024
 */

#include <stdarg.h>
#include <mem/vmm.h>
#include <io/tty.h>
#include <sys/scheduler.h>
#include <io/ports.h>
#include <io/status_loggers.h>
#include <drv/fpu.h>
#include <lib/math.h>
#include <io/rgb_image.h>
#include "lib/sprintf.h"
#include "drv/psf.h"

// Size
size_t tty_current_width = 0;
size_t tty_current_height = 0;

// Position
size_t tty_current_column = 0;
size_t tty_current_row = 0;

size_t tty_current_color = 0;

uint32_t* tty_character_buffer = 0;
uint32_t* tty_attributes_buffer = 0;

uint32_t tty_color_table[128] = {
        [31] = 0xff0000, // Red,
        [32] = 0x00ff00, // Green,
        [33] = 0xffff00, // Yellow,
        [34] = 0x0000ff, // Blue,
        [35] = 0xff00ff, // Magenta,
        [36] = 0x00ffff, // Cyan,
        [37] = 0x808000, // Gray,
};

void tty_init(void) {
    tty_current_width = getScreenWidth() / 8;
    tty_current_height = getScreenHeight() / 16;

    qemu_log("TTY: %d x %d (%d characters)", tty_current_width, tty_current_height, tty_current_width * tty_current_height);

    tty_reset_position();
    tty_build_buffers();

    tty_current_color = 0xffffff;
}

void tty_reset_position() {
    tty_current_column = 0;
    tty_current_row = 0;
}

void tty_clear_buffers() {
    memset(tty_character_buffer, 0, tty_current_width * tty_current_height);
    memset(tty_attributes_buffer, 0, tty_current_width * tty_current_height);
}

void tty_clear() {
    clean_screen();

    tty_clear_buffers();
    tty_reset_position();
}

void tty_build_buffers() {
    tty_character_buffer = kcalloc(sizeof(uint32_t), tty_current_width * tty_current_height);
    tty_attributes_buffer = kcalloc(sizeof(uint32_t), tty_current_width * tty_current_height);
}

void tty_set_color(uint32_t color) {
    tty_current_color = color;
}

uint32_t tty_get_color() {
    return tty_current_color;
}

void _tty_putc(int c) {
    if(tty_current_column >= tty_current_width) {
        tty_current_column = 0;
        tty_current_row++;
    }

    if(tty_current_row >= tty_current_height) {
        tty_scroll();

        tty_current_row = tty_current_height - 1;
        tty_current_column = 0;
    }

    if (c == '\n') {
        tty_current_column = 0;
        tty_current_row++;
        return;
    }

    tty_character_buffer[tty_current_row * tty_current_width + tty_current_column] = c;

    tty_current_column++;
}

void tty_putc(int c) {
    _tty_putc(c);
    tty_render();
}

void tty_render() {
    for (size_t i = 0; i < tty_current_width * tty_current_height; i++) {
        uint32_t character = tty_character_buffer[i];

        if(!character)
            continue;

        if(tty_attributes_buffer[i] & TTY_ATTRIBUTE_COLOR) {
            uint32_t color = tty_attributes_buffer[i] & 0xffffff;

            tty_set_color(color);
        }

        draw_vga_ch(
                character & 0xff,
                character >> 8,
                (i % tty_current_width) * 8,
                (i / tty_current_width) * 16,
                tty_current_color
                );
    }

    punch();
}

void _tty_puts(const char *str) {
    while(*str) {
        if(*str++ == 0x1b) {
            uint32_t attr = 0;

            if(*str++ == '[') {
                size_t code = 0;

                while(*str && *str != 'm') {
                    code = code * 10 + (*str++ & 0xff) - '0'; // 0-9
                }

                str++; // Skip the 'm'

                if(code == 0) {
                    attr |= 0xffffff;
                } else {
                    attr |= tty_color_table[code];
                }

                attr |= TTY_ATTRIBUTE_COLOR;

                qemu_log("Wrote attribute at index %d", tty_current_row * tty_current_width + tty_current_column);
                tty_attributes_buffer[tty_current_row * tty_current_width + tty_current_column] = attr;
            }
        } else {
            str--;
        }

        uint16_t c = (uint16_t)(*str++ & 0xff);

        if(c == 0xD0 || c == 0xD1) { // A kludge for UTF-8 support (only cyrillic)
            c |= (*str++ & 0xff) << 8;
        }

        _tty_putc(c);
    }
}

void tty_puts(const char *str) {
    _tty_puts(str);
    tty_render();
}

void _tty_vprintf(const char* format, va_list args) {
    char* a = 0;

    vasprintf(&a, format, args);

    _tty_puts(a);

    kfree(a);
}

void tty_vprintf(const char* format, va_list args) {
    _tty_vprintf(format, args);
    tty_render();
}

void tty_set_x(uint32_t x) {
    tty_current_column = x;
}

void tty_set_y(uint32_t y) {
    tty_current_row = y;
}

void tty_scroll() {
    memcpy(tty_character_buffer,
           tty_character_buffer + tty_current_width,
           sizeof(uint32_t) * tty_current_width * (tty_current_height - 1)
           );

    memcpy(tty_attributes_buffer,
           tty_attributes_buffer + tty_current_width,
           sizeof(uint32_t) * tty_current_width * (tty_current_height - 1)
           );

    memset(tty_character_buffer + (tty_current_width * (tty_current_height - 1)),
            0,
            tty_current_width * sizeof(uint32_t)
            );

    memset(tty_attributes_buffer + (tty_current_width * (tty_current_height -1)),
           0,
           tty_current_width * sizeof(uint32_t)
           );

    clean_screen();
    tty_render();
}