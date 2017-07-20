
#include <nanosoft/utils.h>
#include <sys/time.h>

/**
 * Вернуть время в миллисекундах
 */
int64_t millitime()
{
	timeval tv;
	gettimeofday(&tv, 0);
	return millitime(tv);
}

/**
 * Конвертировать структуру timeval в миллисекунды
 */
int64_t millitime(const struct timeval &tv)
{
	int64_t ts = tv.tv_sec;
	return ts * 1000 + tv.tv_usec / 1000;
}

/**
 * Вернуть время в микросекундах
 */
int64_t microtime()
{
	timeval tv;
	gettimeofday(&tv, 0);
	return microtime(tv);
}

/**
 * Конвертировать структуру timeval в микросекунды
 */
int64_t microtime(const struct timeval &tv)
{
	int64_t ts = tv.tv_sec;
	return ts * 1000000 + tv.tv_usec;
}
