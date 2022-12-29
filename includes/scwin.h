#pragma once

#include <stdint.h>

typedef struct scwin_req {
	uint32_t height;
	uint32_t width; 
	uint16_t bpp; 
	char *title; 
} scwin_req_t, *scwin_req_ptr;


