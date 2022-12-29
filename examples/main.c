#include <scwin.h>

#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GL/gl.h>

int i = 0;

void key_press() {
	printf("Key was pressed\n");
	return;
}

void draw_frame() {
	glClearColor(1.0, 0.0, 0.3, 0.8f);
	glClear(GL_COLOR_BUFFER_BIT);
	i = 1;
}

int main() {
	scwin_ptr window = NULL;
	EGLDisplay *display = NULL;
	EGLSurface *surface = NULL;
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

	context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
	
	surface = scwin_create_egl_surface(display, config, window, NULL);

	eglMakeCurrent(display, surface, surface, context);

	scwin_map(window);

	display = scwin_create_egl_display(window);

	if(!eglInitialize(display, &major, &minor)) {
		printf("Failed to init egl\n");
		return -1;
	}
	printf("EGL init %d.%d\n", major, minor);

	if(!eglChooseConfig(display, config_attribs, &config, 1, &count)) {
		printf("Failed to choose egl config\n");
		return -1;
	}
	
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
	
	surface = scwin_create_egl_surface(display, config, window, NULL);

	eglMakeCurrent(display, surface, surface, context);


	scwin_set_key_press_fn(window, key_press);
	scwin_set_draw_fn(window, draw_frame);
	while(!scwin_should_close(window)) {
		scwin_poll_events(window);
		//Any of your code here
		if(i) {
			eglSwapBuffers(display, surface);
			i = 0;
		}
	}

	scwin_destroy(window);

	return 0;
}
