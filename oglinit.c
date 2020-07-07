#include <GL/glut.h>
#include "eglstate.h"
#include <assert.h>
#include <VG/openvg.h>

DisplayFunc callback = NULL;
int lastdraw = 0;
int timeinit = 0;//Really just a bool, but C doesn't do those...

//Callback from GLUT for every frame it wants to display
void
callbackDisplay(void)
{
   /* Get interval from last redraw */
   int now = glutGet(GLUT_ELAPSED_TIME);
   if (!timeinit)
   {
      lastdraw = now;
      timeinit = 1;
   }
   unsigned int msinterval = (unsigned int) (now - lastdraw);
   float interval = (float) msinterval / 1000;
   lastdraw = now;

   /* Draw scene */
   if (callback)
      (*callback) (interval);

   //Don't do swap here, it's done by the user calling End()
   //glutSwapBuffers();

}

void setDisplayCallback(DisplayFunc new_callback)
{
	callback = new_callback;
}

void oglSwapBuffers(STATE_T * state)
{
	glutSwapBuffers();
}

void oglMainLoop()
{
	glutMainLoop();
}

void oglfinish(STATE_T * state)
{
	//TODO GLUT cleanup too
	vgDestroyContextSH();
}

int oglNoError()
{
	return glGetError() == GL_NO_ERROR;
}


void
callbackIdle(void)
{
   glutPostRedisplay();
}



// oglinit sets the display, OpenVGL context and screen information
// state holds the display information
// In the new callback version, this is mostly a copy/paste from ShivaVG's testInit
void oglinit(int argc, char **argv, STATE_T * state) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA |
		       GLUT_STENCIL | GLUT_MULTISAMPLE);

	state->screen_width = glutGet(GLUT_SCREEN_WIDTH);
	state->screen_height = glutGet(GLUT_SCREEN_HEIGHT);

	if ((state->window_width == 0) || (state->window_width > state->screen_width))
		state->window_width = state->screen_width;
	if ((state->window_height == 0) || (state->window_height > state->screen_height))
		state->window_height = state->screen_height;

	// adjust position so that at least one pixel is on screen and
	// set up the dispman rects

	glutInitWindowPosition(state->window_x, state->window_y);
	glutInitWindowSize(state->window_width, state->window_height);
	glutCreateWindow("OpenVG");

	//glutReshapeFunc(testReshape);
	glutDisplayFunc(callbackDisplay);
	glutIdleFunc(callbackIdle);
	glutSetCursor(GLUT_CURSOR_NONE);
	//glutKeyboardFunc(testKeyboard);
	//glutSpecialFunc(testSpecialKeyboard);
	//glutMouseFunc(testButton);
	//glutMotionFunc(testDrag);
	//glutPassiveMotionFunc(testMove);
	//atexit(testCleanup);

	vgCreateContextSH(state->window_width, state->window_height);
}

// dispmanMoveWindow repositions the openVG window to given coords
// -ve coords are allowed upto (1-width,1-height),
// max (screen_width-1,screen_height-1). i.e. at least one pixel must be
// on the screen.
void dispmanMoveWindow(STATE_T * state, int x, int y) {
	glutPositionWindow(x,y);
	state->window_x = glutGet(GLUT_WINDOW_X);
	state->window_y = glutGet(GLUT_WINDOW_Y);
	
}

// dispmanChangeWindowOpacity changes the window's opacity
// 0 = transparent, 255 = opaque
void dispmanChangeWindowOpacity(STATE_T * state, uint32_t alpha) {
}
