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

scwin_ptr scwin_create_drm_backend(scwin_req_ptr req) {
	int fd = open("/dev/tty2", O_RDWR | O_CLOEXEC);
	printf("%m\n");
	ioctl(fd, VT_LOCKSWITCH, 1);
	printf("%m\n");
	while(1) {

	}
	char *data = NULL;

	data[0] = 'a';

	return NULL;
}

int main(int argc, char **argv) {
	scwin_create_drm_backend(NULL);
}
