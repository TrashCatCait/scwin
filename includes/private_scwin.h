#pragma once
/*
 * This file is inteded as a private header for common 
 * structures that will be used internally 
 * Technically you can use this if you don't want to use 
 * Opaque pointers but we reccommend using scwin.h 
 * and using the built in helper functions to achieve 
 * what you want 
 */

#ifndef DISABLE_PRIVSCWIN_WARN
#warning "Warning private_scwin.h included please use scwin.h instead"
#endif

/* Note on these structures 
 * These are just common parts of the all the different 
 * backend structures they aren't intended to be used 
 * externally 
 */

#include <stdint.h>
#include <scwin.h>
#include <vulkan/vulkan.h>
#include <EGL/egl.h>

typedef struct scwin_events {
	void (*key_press_event)(void *data);
	void (*key_release_event)(void *data);
	void (*resize_event)(void *data);
	void (*pointer_motion)(void *data);
	void (*pointer_button_click)(void *data);
	void (*pointer_button_release)(void *data);
	void (*draw_event)(void *data);
	void (*enter_event)(void *data);
	void (*leave_event)(void *data);
} scwin_events_t;

struct scwin {
	int close;
	void *data;
	void (*start)(scwin_ptr window);
	void (*destroy)(scwin_ptr window);
	void (*poll_event)(scwin_ptr window);
	
	scwin_events_t events;

	//VK/GL Helpers 
	EGLDisplay *(*scwin_get_egl_display)(scwin_ptr widnow);
	EGLSurface *(*scwin_create_egl_surface)(EGLDisplay *display, EGLConfig config, scwin_ptr window, const EGLint *attrib_list);
	VkResult (*scwin_create_vk_surface)(VkInstance instance, scwin_ptr window, VkAllocationCallbacks allocator, VkSurfaceKHR *surface);
};


