#pragma once

#include <stdint.h>

enum SCWIN_LOGGER_LEVELS {
	SCWIN_LOGGER_OFF = 0,
	SCWIN_LOGGER_FATAL,
	SCWIN_LOGGER_ERROR,
	SCWIN_LOGGER_WARNING,
	SCWIN_LOGGER_DEBUG,
	SCWIN_LOGGER_INFO,
};


void scwin_logger_set_level(int32_t level);
int scwin_logf(int level, char *fmt, ...);	
