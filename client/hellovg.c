// first OpenVG program
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

int width, height;

void DisplayCallback(float interval)
{
	char hello1[] = {'H','e','j',',',' ','v', 0xc3, 0xa4,'r' , 'l','d' ,'e','n',0};
	char hello2[] = {'H','e','l','l',0xc3,0xb3,' ', 'V', 'i', 'l', 0xc3,0xa1,'g',0};
	char hello3[] = {'A','h','o','j',' ','s','v',0xc4,0x95,'t','e',0};

	Start(width, height);				   // Start the picture
	Background(0, 0, 0);				   // Black background
	Fill(44, 77, 232, 1);				   // Big blue marble
	Circle(width / 2, 0, width);			   // The "world"
	Fill(255, 255, 255, 1);				   // White text
	TextMid(width / 2, (height * 0.7), "hello, world", SerifTypeface, width / 15);	// Greetings 
	TextMid(width / 2, (height * 0.5), hello1 , SerifTypeface, width / 15);
	TextMid(width / 2, (height * 0.3), hello2 , SerifTypeface, width / 15);
	TextMid(width / 2, (height * 0.1), hello3 , SerifTypeface, width / 15);
	End();						   // End the picture
}

void KeyCallback(unsigned char key, int x, int y)
{
	//Quit on any key pressed
	finish();					   // Graphics cleanup
	exit(0);
}

int main(int argc, char**argv) {
	init(&argc, argv, &width, &height);				   // Graphics initialization
	MainLoop(DisplayCallback, KeyCallback);
}
