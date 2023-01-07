#pragma once

#ifdef SCWIN_EXPOSE_NATIVE 

#include <scwin.h>
#include <xcb/xcb.h>

enum scwin_backend_types {
	SCWIN_X11;
	SCWIN_WAYLAND;
	SCWIN_DRM;
};


struct scwin_xcb_display {
	xcb_connection_t *connection;
	xcb_window_t window;
}; 

union scwin_display_backends {
	struct scwin_xcb_display xcb;
};

int scwin_get_backend_type(scwin_ptr window);
char *scwin_get_backend_name(scwin_ptr window);
union scwin_display_backends scwin_get_backend_display(scwin_ptr window);
struct scwin_xcb_backend scwin_get_xcb_display(scwin_ptr window);

#endif 
