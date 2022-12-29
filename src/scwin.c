#include <stdlib.h>
#include <stdint.h>
#include <private_scwin.h>
#include <scwin.h>


#define DISABLE_PRIVSCWIN_WARN


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

}

scwin_ptr _scwin_create_from_env(scwin_req_ptr req) {

}

scwin_ptr scwin_create(scwin_req_ptr req) {
	char *name = getenv("SCWIN_OVERRIDE");

	if(name) {
		return _scwin_create_from_name(name, req);
	} 

	return _scwin_create_from_env(req);
}

