#include <io/ports.h>
#include <drv/beeper.h>
#include <io/status_loggers.h>
#include <io/status_sounds.h>
#include <io/tty.h>

void tty_error(char* format, ...)
{
    uint32_t orig = tty_get_color();
    
    ERROR_sound();
    tty_set_color(COLOR_ERROR);

    va_list args;
    va_start(args, format);

    tty_vprintf(format, args);

    va_end(args);

    tty_set_color(orig);
}

void tty_attention(char* format, ...)
{
    uint32_t orig = tty_get_color();

    ATTENTION_sound();
    tty_set_color(COLOR_ATENTION);

    va_list args;
    va_start(args, format);

    tty_vprintf(format, args);

    va_end(args);

    tty_set_color(orig);
}

void tty_alert(char* format, ...)
{
    uint32_t orig = tty_get_color();

    ALERT_sound();
    tty_set_color(COLOR_ALERT);

    va_list args;
    va_start(args, format);

    tty_vprintf(format, args);

    va_end(args);

    tty_set_color(orig);
}

void tty_global_error(char* format, ...)
{
    uint32_t orig = tty_get_color();

    GLOBAL_ERROR_sound();
    tty_set_color(COLOR_ERROR);

    va_list args;
    va_start(args, format);

    tty_vprintf(format, args);

    va_end(args);

    tty_set_color(orig);
}