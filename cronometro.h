#ifndef CRONOMETRO_H
#define CRONOMETRO_H

#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdint.h>
#include <stdio.h>

#if defined(__APPLE__)
#include <mach/mach_time.h>

typedef uint64_t Cronometro;

static inline Cronometro iniciarCronometro(void) {
    return mach_absolute_time();
}

static inline long long tempoDecorridoNs(Cronometro inicio) {
    static mach_timebase_info_data_t base = {0, 0};
    if (base.denom == 0) mach_timebase_info(&base);
    uint64_t ticks = mach_absolute_time() - inicio;
    return (long long)((long double)ticks * base.numer / base.denom);
}

#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef LARGE_INTEGER Cronometro;

static inline Cronometro iniciarCronometro(void) {
    Cronometro inicio;
    QueryPerformanceCounter(&inicio);
    return inicio;
}

static inline long long tempoDecorridoNs(Cronometro inicio) {
    static LARGE_INTEGER frequencia = {0};
    if (frequencia.QuadPart == 0) QueryPerformanceFrequency(&frequencia);
    LARGE_INTEGER fim;
    QueryPerformanceCounter(&fim);
    return (long long)((long double)(fim.QuadPart - inicio.QuadPart) *
                       1000000000.0L / frequencia.QuadPart);
}

#else
#include <time.h>

typedef struct timespec Cronometro;

static inline Cronometro iniciarCronometro(void) {
    Cronometro inicio;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    return inicio;
}

static inline long long tempoDecorridoNs(Cronometro inicio) {
    Cronometro fim;
    clock_gettime(CLOCK_MONOTONIC, &fim);
    return (long long)(fim.tv_sec - inicio.tv_sec) * 1000000000LL +
           (long long)(fim.tv_nsec - inicio.tv_nsec);
}
#endif

static inline void imprimirTempo(const char* operacao, long long nanossegundos) {
    printf("  Tempo %s: %.6f ms (%lld ns)\n", operacao,
           nanossegundos / 1000000.0, nanossegundos);
}

#endif