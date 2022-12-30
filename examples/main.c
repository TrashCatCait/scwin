#include <scwin.h>

#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GL/gl.h>

int i = 0;

void key_press(void *data) {
	printf("Key was pressed\n");
	return;
}


struct egl {
	EGLDisplay *display;
	EGLSurface *surface;
};

void draw_fn(void *data) {
	struct egl *egl = data;

	glClearColor(1.0, 0.0, 0.3, 0.8f);
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(egl->display, egl->surface);
}

int main() {
	scwin_ptr window = NULL;
	struct egl *egl = malloc(sizeof(*egl));

	EGLConfig config;
	EGLContext context;
	EGLint count, major, minor;
	//Describe the sort of config we want to find 
	//helping to disqualify incompatible ones 
	static const EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	//Describe the context attributes 
	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	
	window = scwin_create(NULL);

	scwin_map(window);

	egl->display = scwin_create_egl_display(window);

	if(!eglInitialize(egl->display, &major, &minor)) {
		printf("Failed to init egl\n");
		return -1;
	}
	printf("EGL init %d.%d\n", major, minor);

	if(!eglChooseConfig(egl->display, config_attribs, &config, 1, &count)) {
		printf("Failed to choose egl config\n");
		return -1;
	}
	
	context = eglCreateContext(egl->display, config, EGL_NO_CONTEXT, context_attribs);
	
	egl->surface = scwin_create_egl_surface(egl->display, config, window, NULL);

	eglMakeCurrent(egl->display, egl->surface, egl->surface, context);

	scwin_set_user_data(window, egl);
	scwin_set_draw_fn(window, draw_fn);
	scwin_set_key_press_fn(window, key_press);
	while(!scwin_should_close(window)) {
		scwin_poll_events(window);
		//Any of your code here
	}

	eglWaitClient();
	eglDestroySurface(egl->display, egl->surface);
	eglDestroyContext(egl->display, context);
	eglTerminate(egl->display);
	free(egl);
	//Note this does not destroy any VK or EGL objects you made those
	//Need to be cleaned up with their respective handles 
	scwin_destroy(window);

	return 0;
}
