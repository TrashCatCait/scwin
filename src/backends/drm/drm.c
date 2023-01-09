#include <scwin.h>
#include <backends/drm/drm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>



/*
 * Okay so a bit of spiel here 
 * Basically drm isn't really a "window system"
 * but we choose to support apps using it 
 * To allow for apps to run on DRM backends 
 *
 * Some Events do not apply like 
 * Enter and Leave surface events 
 * 
 * Apps should:
 *
 * A choose to support DRM with TTY switching 
 * B Set the no_tty_switch flag in scwin_req_ptr
 * C Or Set the no_drm flag in scwin_req_ptr 
 */

/*
 * Also we copy tty's where we copy one framebuffer 
 * to multiple screens simply because windows 
 * only have one surface normally.
 */

/*
 * So if we locked the VT switching we maybe 
 * want to handle segfaults.
 *
 * As if the VT switching is locked and we segfault 
 *
 */

typedef struct scwin_drm {
	int fd;
	
	drmModeResPtr res;
	drmModePlaneResPtr plane_res;

	struct gbm_device *device;
	struct gbm_surface *surface;

} scwin_drm_t;

#include <sys/ioctl.h>
#include <linux/vt.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void scwin_drm_cleanup(scwin_ptr backend) {
	scwin_drm_t *drm = (void *)backend;
	


	free(drm);
}

scwin_ptr scwin_create_drm(scwin_req_ptr req) {
	scwin_drm_t *drm = calloc(1, sizeof(*drm));
	if(!drm) {
		return NULL; 
	}

	

	return (void*)drm; 
}

