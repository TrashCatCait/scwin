#include <scwin.h>
#include <stdint.h>
#define DISABLE_PRIVSCWIN_WARN
#include <private_scwin.h>

#include <stdlib.h>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <backends/wayland/xdg-shell.h>

typedef struct {
	scwin_ptr impl;
	
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

	int32_t width;
	int32_t height;
} scwin_wl_t, *scwin_wl_ptr;



scwin_ptr scwin_wl_create(scwin_req_ptr req) {
	scwin_wl_ptr wl = calloc(1, sizeof(*wl));
	
	
	return (void *)wl;
}
