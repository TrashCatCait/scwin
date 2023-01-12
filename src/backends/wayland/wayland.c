#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <EGL/eglext.h>

#include <scwin.h>
#include <stdint.h>
#include <string.h>
#include <wayland-egl-core.h>
#include <wayland-util.h>
#include <wayland-egl.h>
#define DISABLE_PRIVSCWIN_WARN
#include <private_scwin.h>

#include <stdlib.h>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <backends/wayland/xdg-shell.h>
#include <logger/logger.h>

typedef struct {
	scwin_t impl;
	
	struct wl_display *display;
	struct wl_registry *registry;

	//Global WL objects 
	struct wl_compositor *compositor;
	struct wl_shm *shm; 
	struct wl_seat *seat;

	//Global XDG Objects
	struct xdg_wm_base *xdg_wm_base;

	//Client WL Objects 
	struct wl_surface *wl_surface;
	struct wl_pointer *pointer;
	struct wl_keyboard *keyboard; 

	//Client XDG Objects 
	struct xdg_toplevel *xdg_toplevel;
	struct xdg_surface *xdg_surface;

	//For EGL 
	struct wl_egl_window *egl_window;

	int32_t width;
	int32_t height;
} scwin_wl_t, *scwin_wl_ptr;

/* XDG WM BASE CODE */
void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial) {
	xdg_wm_base_pong(xdg_wm_base, serial);
}

struct xdg_wm_base_listener xdg_wm_base_listener = {
	.ping = xdg_wm_base_ping,
};

/* WL SHM CODE */
void wl_shm_format(void *data, struct wl_shm *shm, uint32_t format) {
	scwin_logf(SCWIN_LOGGER_INFO, "Format: %.*s\n", 4, (char *)&format);
}

struct wl_shm_listener wl_shm_listener = {
	.format = wl_shm_format, 
};

/* WL SEAT CODE */
void wl_seat_capabilities(void *data, 
		struct wl_seat *seat, uint32_t caps) {
	scwin_logf(SCWIN_LOGGER_INFO, "Seat capabilities: %d\n", caps);
}

void wl_seat_name(void *data, struct wl_seat *seat, 
		const char *name) {
	scwin_logf(SCWIN_LOGGER_INFO, "Seat Name: %s\n", name);
}

struct wl_seat_listener wl_seat_listener = {
	.capabilities = wl_seat_capabilities,
	.name = wl_seat_name,
};

static void wl_registry_global(void *data, struct wl_registry *registry, 
		uint32_t name, const char *interface, uint32_t version) {
	scwin_wl_ptr wl = data;

	scwin_logf(SCWIN_LOGGER_DEBUG, "%s:\n"
			"\tname: %d\n"
			"\tinterface: %s\n"
			"\tversion: %u\n\n", 
			__FUNCTION__, name,
			interface, version);

	if(strcmp(interface, wl_compositor_interface.name) == 0) {
		wl->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);
	} else if(strcmp(interface, wl_shm_interface.name) == 0) {
		wl->shm = wl_registry_bind(registry, name, &wl_shm_interface, version);
		wl_shm_add_listener(wl->shm, &wl_shm_listener, wl);

	} else if(strcmp(interface, wl_seat_interface.name) == 0) { 
		wl->seat = wl_registry_bind(registry, name, &wl_seat_interface, version);
		wl_seat_add_listener(wl->seat, &wl_seat_listener, wl);		
	} else if(strcmp(interface, xdg_wm_base_interface.name) == 0) {
		wl->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
		xdg_wm_base_add_listener(wl->xdg_wm_base, &xdg_wm_base_listener, NULL);
	}
}

static void wl_registry_global_remove(void *data, struct wl_registry *registry, 
		uint32_t id) {
	//Left blank 
}

static struct wl_registry_listener registry_listener = {
	.global = wl_registry_global,
	.global_remove = wl_registry_global_remove,
};

/* scwin call backs */
void scwin_wl_start(scwin_ptr window) {
	scwin_wl_ptr wl = window; 
	
	wl_surface_commit(wl->wl_surface);	
}

void scwin_wl_destory(scwin_ptr window) {

}

EGLDisplay *scwin_wl_create_egl_display(scwin_ptr window){
	EGLDisplay *display = NULL;
	scwin_wl_ptr wl = (void*)window;
	PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = NULL;
	
	get_platform_display = (void*)eglGetProcAddress("eglGetPlatformDisplayEXT");		
	if(get_platform_display) {
		display = get_platform_display(EGL_PLATFORM_WAYLAND_EXT, wl->display, NULL);
	}

	return display;
}

EGLSurface *scwin_wl_create_egl_surface(EGLDisplay *display, EGLConfig config, scwin_ptr window, const EGLint *attrib_list) {
	scwin_wl_ptr wl = (void*)window;
	
	//eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
	return eglCreateWindowSurface(display, config, wl->egl_window, attrib_list);
}

void xdg_surface_configure(void *data, struct xdg_surface *surface, uint32_t serial) {
	scwin_wl_ptr wl = data;
	printf("configure\n");
	xdg_surface_ack_configure(surface, serial);

	wl->impl.events.draw_event(wl->impl.data);
	wl_surface_commit(wl->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
	.configure = xdg_surface_configure,
};


void scwin_wl_poll_event(scwin_ptr window) {
	scwin_wl_ptr wl = (void *)window; 

	while(wl->impl.close == 0 && wl_display_dispatch(wl->display) != -1) {
		
	}
}

scwin_ptr scwin_create_wl(scwin_req_ptr req) {
	scwin_wl_ptr wl = calloc(1, sizeof(*wl));
	int ret = 0;

	wl->display = wl_display_connect(NULL);	
	if (!wl->display) {
		scwin_logf(SCWIN_LOGGER_FATAL, "scwin: failed to create wayland backend\n");
		free(wl);
		return NULL;
	}	

	wl->registry = wl_display_get_registry(wl->display);
	if(!wl->registry) {
		scwin_logf(SCWIN_LOGGER_FATAL, "scwin: failed to get display registry\n");
		wl_display_disconnect(wl->display);
		free(wl);
		return NULL;
	}

	ret = wl_registry_add_listener(wl->registry, &registry_listener, wl);
	if(ret != 0) {
		scwin_logf(SCWIN_LOGGER_FATAL, "scwin: failed to add registry listener\n");
		wl_registry_destroy(wl->registry);
		wl_display_disconnect(wl->display);
		free(wl);
		return NULL;
	}
	

	wl_display_roundtrip(wl->display);

	wl->wl_surface = wl_compositor_create_surface(wl->compositor);
	wl->xdg_surface = xdg_wm_base_get_xdg_surface(wl->xdg_wm_base, wl->wl_surface);
	wl->xdg_toplevel = xdg_surface_get_toplevel(wl->xdg_surface);

	xdg_surface_add_listener(wl->xdg_surface, &xdg_surface_listener, wl);
	
	//Create a wayland egl window interface so that we can use it 
	//If the user where to call scwin_create_egl_surface 
	wl->egl_window = wl_egl_window_create(wl->wl_surface, 640, 480);

	wl->impl.close = 0;
	wl->impl.destroy = scwin_wl_destory; 
	wl->impl.start = scwin_wl_start; 
	wl->impl.poll_event = scwin_wl_poll_event;
	wl->impl.scwin_get_egl_display = scwin_wl_create_egl_display; 
	wl->impl.scwin_create_egl_surface = scwin_wl_create_egl_surface;
	return (void *)wl;
}
