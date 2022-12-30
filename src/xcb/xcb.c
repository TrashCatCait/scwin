
#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#define DISABLE_PRIVSCWIN_WARN
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

#include <private_scwin.h>
#include <scwin.h>
#include <xcb/xproto.h>

typedef struct scwin_xcb {
	scwin_t impl;
	
	xcb_connection_t *connection;
	xcb_window_t root;
	xcb_window_t window;
	xcb_screen_t *screen;
	xcb_visualid_t visual_id;
	xcb_visualtype_t *visual;
	xcb_colormap_t colormap;

	//Atoms 
	xcb_intern_atom_reply_t *wm_proto; 
	xcb_intern_atom_reply_t *wm_del; 

	//window details 
	uint8_t bpp;
	uint32_t width; 
	uint32_t height;
	char *title;
} scwin_xcb_t, *scwin_xcb_ptr;

void scwin_destroy_xcb(scwin_ptr scwin) {
	scwin_xcb_ptr xcb = (void *)scwin;

	xcb_destroy_window(xcb->connection, xcb->window);

	free(xcb->wm_del);

	free(xcb->wm_proto);

	xcb_free_colormap(xcb->connection, xcb->colormap);

	xcb_disconnect(xcb->connection);

	free(scwin);
}

void scwin_start_xcb(scwin_ptr scwin) {
	scwin_xcb_ptr xcb = (void *)scwin;

	xcb_map_window(xcb->connection, xcb->window);
	xcb_flush(xcb->connection);
}

void scwin_xcb_client_message(scwin_xcb_ptr xcb, xcb_client_message_event_t *event) {
	if(event->data.data32[0] == xcb->wm_del->atom) {
		xcb->impl.close = 1;
	}
}

/* 
 * TODO all events
 */
void scwin_xcb_key_press(scwin_xcb_ptr xcb, xcb_key_press_event_t *event) {	
	if(xcb->impl.key_event) {
		xcb->impl.key_event(xcb->impl.data);
	}
}

void scwin_xcb_expose(scwin_xcb_ptr xcb, xcb_expose_event_t *event) {
	if(xcb->impl.draw_event) {
		xcb->impl.draw_event(xcb->impl.data);
	}
}

void scwin_xcb_event(scwin_xcb_ptr xcb, xcb_generic_event_t *event) {
	switch (event->response_type & 0x7f) {
		case XCB_EXPOSE:
			scwin_xcb_expose(xcb, (void*)event);
			break;
		case XCB_KEY_PRESS: 
			scwin_xcb_key_press(xcb, (void*)event);
			break;
		case XCB_CLIENT_MESSAGE:
			scwin_xcb_client_message(xcb, (void*)event);
			break;
		default:
			printf("Unknown Event %d\n", event->response_type & 0x7f);
			break;
	}
}

void scwin_poll_xcb(scwin_ptr scwin) {
	scwin_xcb_ptr xcb = (void*)scwin;
	xcb_generic_event_t *event = NULL;

	//Just sit and get events for now
	while((event = xcb_poll_for_event(xcb->connection))) {
		scwin_xcb_event(xcb, event);
		free(event);
	}
}

scwin_result_t scwin_xcb_get_screen(int screen_no, const xcb_setup_t *setup, xcb_screen_t **screen_out) {
	xcb_screen_iterator_t iter = { 0 };

	iter = xcb_setup_roots_iterator(setup);
	
	for(; iter.rem; screen_no--, xcb_screen_next(&iter)) {
		if(screen_no == 0) {
			*screen_out = iter.data;
		}
	}
	if(*screen_out) {
		return SCWIN_SUCESS;
	}

	return SCWIN_BACKEND_ERROR;
}

/*
 * Matches a visual and returns it if found
 */
xcb_visualtype_t *scwin_xcb_match_visual(xcb_screen_t *screen, uint8_t bpp, xcb_visual_class_t class) {
	xcb_depth_iterator_t depth_iter;
	xcb_visualtype_iterator_t visual_iter;
	xcb_depth_t *depth = NULL;
	xcb_visualtype_t *visual;
	
	printf("Looking for %d-bit visual with class: %d\n", bpp, class);
	depth_iter = xcb_screen_allowed_depths_iterator(screen);
	for(; depth_iter.rem; xcb_depth_next(&depth_iter)) {
		if(depth_iter.data->depth != bpp || !depth_iter.data->visuals_len) {
			continue;
		}
		printf("Depth: %d Visuals: %d\n", depth_iter.data->depth, depth_iter.data->visuals_len);
		depth = depth_iter.data;
	}
	//return null if there is no 32 bit depths 
	if(!depth) {
		printf("Failed to get depth of 32-bits\n");
		return NULL;
	}


	visual_iter = xcb_depth_visuals_iterator(depth);
	for(; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
		if(visual_iter.data->_class != class) {
			continue;
		}
		/*
		 * Should I maybe check spefically for RGB visuals incase BGR or some other ones exist 
		 */
		visual = visual_iter.data;
	}

	return visual;
}

scwin_result_t scwin_xcb_create_colormap(xcb_connection_t *connection, xcb_window_t window, xcb_visualid_t visual, xcb_colormap_t *colormap) {
	xcb_void_cookie_t cookie = { 0 };
	xcb_generic_error_t *error = NULL;
	
	*colormap = xcb_generate_id(connection);

	cookie = xcb_create_colormap_checked(connection, XCB_COLORMAP_ALLOC_NONE, *colormap, window, visual);
	error = xcb_request_check(connection, cookie);
	if(error) {
		printf("Failed to create color map: %d\n", error->error_code);
		return SCWIN_BACKEND_ERROR;
	}
 

	return SCWIN_SUCESS;
}


EGLDisplay *scwin_xcb_create_egl_display(scwin_ptr window) {
	EGLDisplay *display = NULL;
	scwin_xcb_ptr xcb = (void*)window;
	PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = NULL;
	
	get_platform_display = (void*)eglGetProcAddress("eglGetPlatformDisplayEXT");		
	if(get_platform_display) {
		display = get_platform_display(EGL_PLATFORM_XCB_EXT, xcb->connection, NULL);
	}

	return display;
}

EGLSurface *scwin_xcb_create_egl_surface(EGLDisplay *display, EGLConfig config, scwin_ptr window, const EGLint *attrib_list) {
	scwin_xcb_ptr xcb = (void*)window;

	return eglCreateWindowSurface(display, config, xcb->window, attrib_list);
}

VkResult scwin_xcb_create_vk_surface(VkInstance instance, scwin_ptr window, VkAllocationCallbacks allocator, VkSurfaceKHR *surface) {
	VkXcbSurfaceCreateInfoKHR info = { 0 };
	scwin_xcb_ptr xcb = (void*)window;

	info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	info.window = xcb->window;
	info.connection = xcb->connection;

	return vkCreateXcbSurfaceKHR(instance, &info, &allocator, surface);
}


/* 
 * Create an scwin with the xcb backend 
 */
scwin_ptr scwin_create_xcb(scwin_req_ptr req) {
	scwin_xcb_ptr xcb = calloc(1, sizeof(*xcb));
	const xcb_setup_t *setup;
	xcb_screen_iterator_t screen_iterator;
	int preffered_screen = 0;
	uint32_t window_mask = 0;
	uint32_t window_values[4] = { 0 };
	xcb_generic_error_t *error = NULL;

	xcb->impl.destroy = scwin_destroy_xcb;
	xcb->impl.start = scwin_start_xcb;
	xcb->impl.poll_event = scwin_poll_xcb;

	//Helpers to make easier surfaces for graphics APIs 
	xcb->impl.scwin_create_egl_surface = scwin_xcb_create_egl_surface; 
	xcb->impl.scwin_get_egl_display = scwin_xcb_create_egl_display;
	xcb->impl.scwin_create_vk_surface = scwin_xcb_create_vk_surface;


	xcb->connection = xcb_connect(getenv("DISPLAY"), &preffered_screen);
	
	setup = xcb_get_setup(xcb->connection);

	scwin_xcb_get_screen(preffered_screen, setup, &xcb->screen);

	if(req) {
		xcb->width = req->width;
		xcb->height = req->height;
		xcb->bpp = req->bpp ? req->bpp : 32; //default to 32 if req.bpp is not set 
		xcb->title = req->title ? req->title : "Untitled";	
	} else {
		xcb->title = "Untitled";
		xcb->bpp = 32;
		xcb->width = 640;
		xcb->height = 480;
	}
	
	xcb->visual = scwin_xcb_match_visual(xcb->screen, xcb->bpp, XCB_VISUAL_CLASS_TRUE_COLOR);
	if(xcb->visual) {
		xcb->visual_id = xcb->visual->visual_id;
	} else {
		printf("%d-bit visual could not be found defaulting to root window visual\n", xcb->bpp);
		xcb->visual_id = xcb->screen->root_visual;
		xcb->bpp = xcb->screen->root_depth;
	}
	
	xcb->root = xcb->screen->root;

	scwin_xcb_create_colormap(xcb->connection, xcb->root, xcb->visual_id, &xcb->colormap);
	
	window_values[0] = xcb->screen->black_pixel;
	window_values[1] = XCB_EVENT_MASK_EXPOSURE;
	window_values[2] = xcb->colormap;

	window_mask = XCB_CW_EVENT_MASK | XCB_CW_BORDER_PIXEL | XCB_CW_COLORMAP;

	xcb->window = xcb_generate_id(xcb->connection);

	xcb_create_window(xcb->connection, xcb->bpp, 
			xcb->window, xcb->screen->root, 
			0, 0, xcb->width, xcb->height, 
			1, XCB_WINDOW_CLASS_INPUT_OUTPUT, 
			xcb->visual_id, window_mask, window_values);
	
	xcb_change_property(xcb->connection, XCB_PROP_MODE_REPLACE, 
			xcb->window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(xcb->title), xcb->title);
	
	//Setup WM close messages 
	xcb_intern_atom_cookie_t wm_proto_cookie = xcb_intern_atom(xcb->connection, 1, 12, "WM_PROTOCOLS");	
	xcb_intern_atom_cookie_t wm_del_cookie = xcb_intern_atom(xcb->connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
	xcb->wm_proto = xcb_intern_atom_reply(xcb->connection, wm_proto_cookie, NULL);
	xcb->wm_del = xcb_intern_atom_reply(xcb->connection, wm_del_cookie, NULL);

	xcb_change_property(xcb->connection, XCB_PROP_MODE_REPLACE, 
			xcb->window, (*xcb->wm_proto).atom, 4, 32, 1, &(*xcb->wm_del).atom);

	xcb_flush(xcb->connection);

	return (void *)xcb;
}

