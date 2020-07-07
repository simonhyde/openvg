INCLUDEFLAGS=-I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -fPIC
INCLUDEFLAGS_OGL=-IShivaVG/include -fPIC
LIBFLAGS=-L/opt/vc/lib -lbrcmEGL -lbrcmGLESv2 -ljpeg 
LIBFLAGS_OGL=-LShivaVG/build/src -ljpeg -lOpenVGStatic -lglut -lGL
FONTLIB=/usr/share/fonts/truetype/ttf-dejavu
FONTFILES=DejaVuSans.inc  DejaVuSansMono.inc DejaVuSerif.inc
all:	$(FONTFILES) library

libshapes.o:	libshapes.c shapes.h fontinfo.h $(FONTFILES)
	gcc -O2 -Wall $(INCLUDEFLAGS) -c libshapes.c

libshapesOGL.o:	libshapes.c shapes.h fontinfo.h $(FONTFILES)
	gcc -O2 -Wall $(INCLUDEFLAGS_OGL) -c libshapes.c -o libshapesOGL.o

gopenvg:	openvg.go
	go install .

rpiinit.o:	rpiinit.c eglstate.h
	gcc -O2 -Wall $(INCLUDEFLAGS) -c rpiinit.c

oglinit.o:	oglinit.c eglstate.h
	gcc -O2 -Wall $(INCLUDEFLAGS_OGL) -c oglinit.c

font2openvg:	fontutil/font2openvg.cpp
	g++ -I/usr/include/freetype2 fontutil/font2openvg.cpp -o font2openvg -lfreetype


DejaVuSans.inc: font2openvg $(FONTLIB)/DejaVuSans.ttf
	./font2openvg $(FONTLIB)/DejaVuSans.ttf DejaVuSans.inc DejaVuSans

DejaVuSerif.inc: font2openvg $(FONTLIB)/DejaVuSerif.ttf
	./font2openvg $(FONTLIB)/DejaVuSerif.ttf DejaVuSerif.inc DejaVuSerif

DejaVuSansMono.inc: font2openvg $(FONTLIB)/DejaVuSansMono.ttf
	./font2openvg $(FONTLIB)/DejaVuSansMono.ttf DejaVuSansMono.inc DejaVuSansMono

clean:
	rm -f *.o *.inc *.so font2openvg *.c~ *.h~
	indent -linux -c 60 -brf -l 132  libshapes.c rpiinit.c shapes.h fontinfo.h

library: libshapes.so libshapesOGL.so

libshapes.so: rpiinit.o libshapes.o
	gcc $(LIBFLAGS) -shared -o libshapes.so rpiinit.o libshapes.o

libshapesOGL.so: oglinit.o libshapesOGL.o ShivaVG/build/src/libOpenVGStatic.a
	gcc $(LIBFLAGS_OGL) -shared -o libshapesOGL.so oglinit.o libshapesOGL.o

ShivaVG/build/src/libOpenVGStatic.a: ShivaVG/build/Makefile
	$(MAKE) -C ShivaVG/build

ShivaVG/build/Makefile: | ShivaVG/build
	cd ShivaVG/build && cmake .. -G "Unix Makefiles" -DBUILD_ALL_EXAMPLES=OFF

ShivaVG/build:
	mkdir ShivaVG/build

install:
	install -m 755 -p font2openvg /usr/bin/
	install -m 755 -p libshapes.so /usr/lib/libshapes.so.1.0.0
	strip --strip-unneeded /usr/lib/libshapes.so.1.0.0
	ln -f -s /usr/lib/libshapes.so.1.0.0 /usr/lib/libshapes.so
	ln -f -s /usr/lib/libshapes.so.1.0.0 /usr/lib/libshapes.so.1
	ln -f -s /usr/lib/libshapes.so.1.0.0 /usr/lib/libshapes.so.1.0
	install -m 644 -p shapes.h /usr/include/
	install -m 644 -p fontinfo.h /usr/include/

uninstall:
	rm -f /usr/bin/font2openvg
	rm -f /usr/lib/libshapes.so.1.0.0 /usr/lib/libshapes.so.1.0 /usr/lib/libshapes.so.1 /usr/lib/libshapes.so
	rm -f /usr/include/shapes.h /usr/include/fontinfo.h
