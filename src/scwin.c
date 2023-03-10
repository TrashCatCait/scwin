#define DISABLE_PRIVSCWIN_WARN
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <private_scwin.h>
#include <scwin.h>
#include <backends/xcb/xcb.h>
#include <backends/wayland/wayland.h>


/*
 * We support two ways of creating a window 
 * One is to just pass in nothing as an argument 
 * And everything will be set to generic defaults 
 * Or more likely you'll want to use a scwin request 
 * in order to request certain characteristics 
 * Width, height, bpp, title, etc
 *
 */

scwin_ptr _scwin_create_from_name(char *name, scwin_req_ptr req) {
	if(strncmp(name, "xcb", 3) == 0) {
		return scwin_create_xcb(req);	
	}
	return NULL;
}

scwin_ptr _scwin_create_from_env(scwin_req_ptr req) {
	
	if(getenv("WAYLAND_DISPLAY")) {
		return scwin_create_wl(req);
	}

	if(getenv("DISPLAY")) {
		return scwin_create_xcb(req);
	}

	return NULL;
}

void scwin_destroy(scwin_ptr window) {
	window->destroy(window);
}

void scwin_poll_events(scwin_ptr window) {
	window->poll_event(window);
}

void scwin_map(scwin_ptr window) {
	window->start(window);
}

int scwin_should_close(scwin_ptr window) {
	return window->close;
}

void scwin_set_key_press_fn(scwin_ptr window, void *fn) {
	window->events.key_press_event = fn;
}

void scwin_set_draw_fn(scwin_ptr window, void *fn) {
	window->events.draw_event = fn;
}

void scwin_set_user_data(scwin_ptr window, void *data) {
	window->data = data;
}


EGLDisplay *scwin_create_egl_display(scwin_ptr window) {
	return window->scwin_get_egl_display(window);
}

EGLSurface *scwin_create_egl_surface(EGLDisplay *display, EGLConfig config, scwin_ptr window, const EGLint *attrib_list) {
	return window->scwin_create_egl_surface(display, config, window, attrib_list);
}

scwin_ptr scwin_create(scwin_req_ptr req) {
	char *name = getenv("SCWIN_OVERRIDE");

	if(name) {
		return _scwin_create_from_name(name, req);
	} 

	return _scwin_create_from_env(req);
}

