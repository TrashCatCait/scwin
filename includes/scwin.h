#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <EGL/egl.h>

typedef struct scwin_req {
	uint32_t height;
	uint32_t width; 
	uint16_t bpp; 
	char *title; 
	void *extension; //Reserved but may be used to extend this 
} scwin_req_t, *scwin_req_ptr;

enum scwin_results {
	SCWIN_SUCESS = 0,
	SCWIN_BACKEND_ERROR = 1,
};


typedef struct scwin scwin_t, *scwin_ptr;

typedef int scwin_result_t;

scwin_ptr scwin_create(scwin_req_ptr req);
void scwin_destroy(scwin_ptr window);
void scwin_poll_events(scwin_ptr window);
void scwin_map(scwin_ptr window);
void scwin_set_key_press_fn(scwin_ptr window, void *fn);
int scwin_should_close(scwin_ptr window);
VkResult scwin_create_vksurface(VkInstance instance, scwin_ptr window, VkAllocationCallbacks allocator, VkSurfaceKHR *surface);
EGLDisplay *scwin_create_egl_display(scwin_ptr window);
EGLSurface *scwin_create_egl_surface(EGLDisplay *display, EGLConfig config, scwin_ptr window, const EGLint *attrib_list);
