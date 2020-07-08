#include <EGL/egl.h>
#include "eglstate.h"
#include <bcm_host.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "key.c"

static DisplayFunc callback = NULL;
static KeyboardFunc keyCallback = NULL;
static struct timeval lasttv;

static int loopRunning = 1;


typedef struct
{
       // dispman window
       DISPMANX_ELEMENT_HANDLE_T element;

       // EGL data
       EGLDisplay display;

       EGLSurface surface;
       EGLContext context;
} GLSTATE_T;


void setKeyboardCallback(KeyboardFunc new_callback)
{
	keyCallback = new_callback;
}

void setDisplayCallback(DisplayFunc new_callback)
{
	callback = new_callback;
}

void oglSwapBuffers(STATE_T * state)
{
       eglSwapBuffers(((GLSTATE_T *)state->platform)->display, ((GLSTATE_T *)state->platform)->surface);
}

void oglMainLoop()
{
	loopRunning = 1;
	int firstPass =1;
	while(loopRunning)
	{
		struct timeval nowtv;
		gettimeofday(&nowtv,NULL);
		if(firstPass)
		{
			lasttv = nowtv;
			firstPass = 0;
		}
		float interval = nowtv.tv_sec - lasttv.tv_sec;
		interval += (nowtv.tv_usec - lasttv.tv_usec)/1000000.0;
		lasttv = nowtv;
		int character = 0;

		if(loopRunning && keyPressed(&character) && keyCallback)
			(*keyCallback) (character, 0, 0);
		if(loopRunning && callback)
			(*callback) (interval);
		else if(!callback)
		{
			fprintf(stderr, "No callback defined, aborting main loop\n");
			break;
		}
	}
}

void oglLeaveMainLoop()
{
	loopRunning = 0;
}

void oglfinish(STATE_T * _state)
{
	loopRunning = 0;//Stop any running callback loop
	GLSTATE_T * state = (GLSTATE_T *)(_state->platform);

	eglSwapBuffers(state->display, state->surface);
	eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(state->display, state->surface);
	eglDestroyContext(state->display, state->context);
	eglTerminate(state->display);
	keyboardReset();
}

int oglNoError()
{
	return eglGetError() == EGL_SUCCESS;
}


// setWindowParams sets the window's position, adjusting if need be to
// prevent it from going fully off screen. Also sets the dispman rects
// for displaying.
static void setWindowParams(STATE_T * state, int x, int y, VC_RECT_T * src_rect, VC_RECT_T * dst_rect) {
	uint32_t dx, dy, w, h, sx, sy;

	// Set source & destination rectangles so that the image is
	// clipped if it goes off screen (else dispman won't show it properly)
	if (x < (1 - (int)state->window_width)) {	   // Too far off left
		x = 1 - (int)state->window_width;
		dx = 0;
		sx = state->window_width - 1;
		w = 1;
	} else if (x < 0) {				   // Part of left is off
		dx = 0;
		sx = -x;
		w = state->window_width - sx;
	} else if (x < (state->screen_width - state->window_width)) {	// On
		dx = x;
		sx = 0;
		w = state->window_width;
	} else if (x < state->screen_width) {		   // Part of right is off
		dx = x;
		sx = 0;
		w = state->screen_width - x;
	} else {					   // Too far off right
		x = state->screen_width - 1;
		dx = state->screen_width - 1;
		sx = 0;
		w = 1;
	}

	if (y < (1 - (int)state->window_height)) {	   // Too far off top
		y = 1 - (int)state->window_height;
		dy = 0;
		sy = state->window_height - 1;
		h = 1;
	} else if (y < 0) {				   // Part of top is off
		dy = 0;
		sy = -y;
		h = state->window_height - sy;
	} else if (y < (state->screen_height - state->window_height)) {	// On
		dy = y;
		sy = 0;
		h = state->window_height;
	} else if (y < state->screen_height) {		   // Part of bottom is off
		dy = y;
		sy = 0;
		h = state->screen_height - y;
	} else {					   // Wholly off bottom
		y = state->screen_height - 1;
		dy = state->screen_height - 1;
		sy = 0;
		h = 1;
	}

	state->window_x = x;
	state->window_y = y;

	vc_dispmanx_rect_set(dst_rect, dx, dy, w, h);
	vc_dispmanx_rect_set(src_rect, sx << 16, sy << 16, w << 16, h << 16);
}

// oglinit sets the display, OpenVGL context and screen information
// state holds the display information
void oglinit(int *pargc, char **argv, STATE_T * state) {
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;
	if(state->platform == NULL)
	{
		state->platform = malloc(sizeof(GLSTATE_T));
		memset(state->platform, 0, sizeof(GLSTATE_T));
	}

	GLSTATE_T * pstate = (GLSTATE_T *)(state->platform);

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;
	static VC_DISPMANX_ALPHA_T alpha = {
		DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
		255, 0
	};

	static const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	EGLConfig config;

	bcm_host_init();

	// get an EGL display connection
	pstate->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(pstate->display != EGL_NO_DISPLAY);

	// initialize the EGL display connection
	result = eglInitialize(pstate->display, NULL, NULL);
	assert(EGL_FALSE != result);

	// bind OpenVG API
	eglBindAPI(EGL_OPENVG_API);

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(pstate->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

	// create an EGL rendering context
	pstate->context = eglCreateContext(pstate->display, config, EGL_NO_CONTEXT, NULL);
	assert(pstate->context != EGL_NO_CONTEXT);

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */ , &state->screen_width,
					    &state->screen_height);
	assert(success >= 0);

	if ((state->window_width == 0) || (state->window_width > state->screen_width))
		state->window_width = state->screen_width;
	if ((state->window_height == 0) || (state->window_height > state->screen_height))
		state->window_height = state->screen_height;

	// adjust position so that at least one pixel is on screen and
	// set up the dispman rects
	setWindowParams(state, state->window_x, state->window_y, &src_rect, &dst_rect);

	dispman_display = vc_dispmanx_display_open(0 /* LCD */ );
	dispman_update = vc_dispmanx_update_start(0);

	dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display, 0 /*layer */ , &dst_rect, 0 /*src */ ,
						  &src_rect, DISPMANX_PROTECTION_NONE, &alpha, 0 /*clamp */ ,
						  0 /*transform */ );

	pstate->element = dispman_element;
	nativewindow.element = dispman_element;
	nativewindow.width = state->window_width;
	nativewindow.height = state->window_height;
	vc_dispmanx_update_submit_sync(dispman_update);

	pstate->surface = eglCreateWindowSurface(pstate->display, config, &nativewindow, NULL);
	assert(pstate->surface != EGL_NO_SURFACE);

	// preserve the buffers on swap
	result = eglSurfaceAttrib(pstate->display, pstate->surface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
	assert(EGL_FALSE != result);

	// connect the context to the surface
	result = eglMakeCurrent(pstate->display, pstate->surface, pstate->surface, pstate->context);
	assert(EGL_FALSE != result);
}

// dispmanMoveWindow repositions the openVG window to given coords
// -ve coords are allowed upto (1-width,1-height),
// max (screen_width-1,screen_height-1). i.e. at least one pixel must be
// on the screen.
void dispmanMoveWindow(STATE_T * state, int x, int y) {
	VC_RECT_T src_rect, dst_rect;
	DISPMANX_UPDATE_HANDLE_T dispman_update;

	setWindowParams(state, x, y, &src_rect, &dst_rect);
	dispman_update = vc_dispmanx_update_start(0);
	vc_dispmanx_element_change_attributes(dispman_update, ((GLSTATE_T*)(state->platform))->element, 0, 0, 0, &dst_rect, &src_rect, 0, DISPMANX_NO_ROTATE);
	vc_dispmanx_update_submit_sync(dispman_update);
}

// dispmanChangeWindowOpacity changes the window's opacity
// 0 = transparent, 255 = opaque
void dispmanChangeWindowOpacity(STATE_T * state, uint32_t alpha) {
	DISPMANX_UPDATE_HANDLE_T dispman_update;

	if (alpha > 255)
		alpha = 235;

	dispman_update = vc_dispmanx_update_start(0);
	// The 1<<1 below means update the alpha value
	vc_dispmanx_element_change_attributes(dispman_update, ((GLSTATE_T*)(state->platform))->element, 1 << 1, 0, alpha, 0, 0, 0, DISPMANX_NO_ROTATE);
	vc_dispmanx_update_submit_sync(dispman_update);
}
