#pragma once

#define DISABLE_PRIVSCWIN_WARN

#include <private_scwin.h>
#include <scwin.h>

	
scwin_ptr scwin_create_xcb(scwin_req_ptr req);
