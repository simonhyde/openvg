// first OpenVG program
// Anthony Starks (ajstarks@gmail.com)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

int main(int argc, char **argv) {
	int width, height;
	init(&argc, argv, &width, &height);				   // Graphics initialization
	printf("%d %d\n", width, height);
	finish();					   // Graphics cleanup
	exit(0);
}
