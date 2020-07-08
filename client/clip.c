//
// clip: test rectangular clipping
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "shapes.h"

static int  w, h, cx, cy, cw, ch, midy, fontsize;
static char *message = "Now is the time for all good men to come to the aid of the party";
static float speed, x, rx, ry, rw, rh;

void callback(float);

void keyCallback(unsigned char key, int x, int y)
{
	finish();
	exit(0);
}

int main(int argc, char **argv) {

	init(&argc, argv, &w, &h);
	speed = 15.0;
	x  = 0;
	midy = (h / 2);
	fontsize = w / 50;
	cx = 0;
	ch = fontsize * 2;
	cw = w;
	cy = midy - (ch / 2);

	rx = (float)cx;
	ry = (float)cy;
	rw = (float)cw;
	rh = (float)ch;

	x = 0.0;
	MainLoop(callback, keyCallback);
}

void callback(float interval)
{
	x += speed;
	// scroll the text, only in the clipping rectangle, go back to the start when we're done
	if (x >= rw + speed)
	{
		x = 0.0;
	}
	Start(w, h);
	Background(255, 255, 255);
	Fill(0, 0, 0, .2);
	Rect(rx, ry, rw, rh);
	ClipRect(cx, cy, cw, ch);
	Translate(x, ry+(fontsize/2));
	Fill(0, 0, 0, 1);
	Text(0, 0, message, SansTypeface, fontsize);
	ClipEnd();
	End();
}
