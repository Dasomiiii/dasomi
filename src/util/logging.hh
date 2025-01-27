#pragma once

#include <stdio.h>


#define LOG_SUCCESS(str, ...) \
    printf("\033[1m\033[32m" str "\033[0m" "\n" __VA_OPT__(,) __VA_ARGS__)


#define LOG_ERROR(str, ...) \
    fprintf(stderr, "\033[1m\033[31m" str "\033[0m" "\n" __VA_OPT__(,) __VA_ARGS__)


#define LOG_WARNING(str, ...) \
    printf("\033[1m\033[33m" str "\033[0m" "\n" __VA_OPT__(,) __VA_ARGS__)
