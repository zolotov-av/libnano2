#ifndef NANOSOFT_UTILS_H
#define NANOSOFT_UTILS_H

#include <stdint.h>

/**
 * Вернуть время в миллисекундах
 */
int64_t millitime();

/**
 * Конвертировать структуру timeval в миллисекунды
 */
int64_t millitime(const struct timeval &tv);

/**
 * Вернуть время в микросекундах
 */
int64_t microtime();

/**
 * Конвертировать структуру timeval в микросекунды
 */
int64_t microtime(const struct timeval &tv);

#endif // NANOSOFT_UTILS_H
