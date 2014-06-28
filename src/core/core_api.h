#ifndef UAECORE11_CORE_API_H
#define UAECORE11_CORE_API_H

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef DLL_EXPORT
    #define UAECORE11API __declspec(dllexport)
  #else
    #define UAECORE11API /*__declspec(dllimport)*/
  #endif
#else
  #define UAECORE11API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned int (*get_byte)(unsigned int address);
    unsigned int (*get_word)(unsigned int address);
    unsigned int (*get_long)(unsigned int address);
    unsigned int (*get_wordi)(unsigned int address);
    unsigned int (*get_longi)(unsigned int address);

    void (*put_byte)(unsigned int address, unsigned int value);
    void (*put_word)(unsigned int address, unsigned int value);
    void (*put_long)(unsigned int address, unsigned int value);

    void (*ticks)(unsigned long ticks);
} uaecore11_callbacks_t;

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

UAECORE11API void uaecore11_init(uaecore11_callbacks_t *callbacks);

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
