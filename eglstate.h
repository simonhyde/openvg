#include "shapes.h"

typedef struct {
	// Screen dimentions
	uint32_t screen_width;
	uint32_t screen_height;
	// Window dimentions
	int32_t window_x;
	int32_t window_y;
	uint32_t window_width;
	uint32_t window_height;
	void * platform;
} STATE_T;


extern void oglinit(int argc, char **argv, STATE_T *);
extern void dispmanMoveWindow(STATE_T *, int, int);
extern void dispmanChangeWindowOpacity(STATE_T *, unsigned int);

extern void setDisplayCallback(DisplayFunc callback);

extern void oglSwapBuffers(STATE_T *);

extern void oglMainLoop();

extern void oglfinish(STATE_T *);

extern int oglNoError();
