#include <logger/logger.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

static int32_t logger_level = 0;

void scwin_logger_set_level(int32_t level) {
	logger_level = level;
}

int scwin_logf(int level, char *fmt, ...) {
	int ret = 0;
	va_list args;
	va_start(args, fmt);
	
	if(level <= logger_level) {
		ret = vfprintf(stderr, fmt, args);
	}

	va_end(args);
	return ret;
}
