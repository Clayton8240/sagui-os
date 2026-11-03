/* console.c - kprintf e klog. */
#include "console.h"

void kprintf(const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    formatar(buf, sizeof buf, fmt, ap);
    va_end(ap);
    for (char *p = buf; *p; p++) {
        vga_char(*p);
        serial_char(*p);
    }
}

void klog(const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    formatar(buf, sizeof buf, fmt, ap);
    va_end(ap);
    serial_texto(buf);
}
