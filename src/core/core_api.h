#ifndef UAECORE11_CORE_API_H
#define UAECORE11_CORE_API_H

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef LIBUAECORE11_BUILD
    #ifdef DLL_EXPORT
      #define UAECORE11API __declspec(dllexport)
    #else
      #define UAECORE11API
    #endif
  #else
    #define UAECORE11API extern __declspec(dllimport)
  #endif
#else
  #ifdef LIBUAECORE11_BUILD
    #define UAECORE11API __attribute__((visibility("default")))
  #else
    #define UAECORE11API extern
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int (*uaecore11_read_handler_t)(unsigned int address);
typedef void (*uaecore11_write_handler_t)(unsigned int address, unsigned int value);
typedef void (*uaecore11_ticks_handler_t)(unsigned long ticks);

typedef struct {
    uaecore11_read_handler_t get_byte;
    uaecore11_read_handler_t get_word;
    uaecore11_read_handler_t get_long;
    uaecore11_read_handler_t get_wordi;
    uaecore11_read_handler_t get_longi;

    uaecore11_write_handler_t put_byte;
    uaecore11_write_handler_t put_word;
    uaecore11_write_handler_t put_long;

    uaecore11_ticks_handler_t ticks;
} uaecore11_handlers_t;

typedef enum {
    UAECORE11_REGISTER_D0 = 0,
    UAECORE11_REGISTER_D1,
    UAECORE11_REGISTER_D2,
    UAECORE11_REGISTER_D3,
    UAECORE11_REGISTER_D4,
    UAECORE11_REGISTER_D5,
    UAECORE11_REGISTER_D6,
    UAECORE11_REGISTER_D7,

    UAECORE11_REGISTER_A0,
    UAECORE11_REGISTER_A1,
    UAECORE11_REGISTER_A2,
    UAECORE11_REGISTER_A3,
    UAECORE11_REGISTER_A4,
    UAECORE11_REGISTER_A5,
    UAECORE11_REGISTER_A6,
    UAECORE11_REGISTER_A7,

    UAECORE11_REGISTER_SSP,
    UAECORE11_REGISTER_USP,

    UAECORE11_REGISTER_PC,
    UAECORE11_REGISTER_SR
} uaecore11_register_t;

UAECORE11API void uaecore11_init(uaecore11_handlers_t *handlers);

UAECORE11API void uaecore11_reset();

UAECORE11API void uaecore11_execute();

UAECORE11API void uaecore11_raise_irq(int level);

UAECORE11API void uaecore11_lower_irq(int level);

UAECORE11API unsigned int uaecore11_get_register(uaecore11_register_t reg);

UAECORE11API void uaecore11_set_register(uaecore11_register_t reg, unsigned int value);

#ifdef __cplusplus
}
#endif

#endif
