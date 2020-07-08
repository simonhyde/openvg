INCLUDEFLAGS=-I/opt/vc/include -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads -I..
INCLUDEFLAGS_OGL=-I../ShivaVG/include -I..
LIBFLAGS=-L/opt/vc/lib -lbrcmEGL -lbrcmGLESv2 -lbcm_host -lpthread  -ljpeg
LIBFLAGS_OGL=-L../ShivaVG/build/src -lOpenVGStatic -lpthread -ljpeg -lm -lglut -lGLEW -lGL -lGLU

#all: shapedemo.OGL hellovg.OGL mouse-hellovg.OGL particles.OGL screensize.OGL clip.OGL \
#     shapedemo.OVG hellovg.OVG mouse-hellovg.OVG particles.OVG screensize.OVG clip.OVG
all: shapedemo.OGL hellovg.OGL particles.OGL screensize.OGL clip.OGL \
     shapedemo.OVG hellovg.OVG particles.OVG screensize.OVG clip.OVG

%.OGL:	%.c ../libshapesOGL.o ../oglinit.o
	gcc -Wall $(INCLUDEFLAGS_OGL)  -o $@ $^ $(LIBFLAGS_OGL)

%.OVG:	%.c ../libshapes.o ../rpiinit.o
	gcc -Wall $(INCLUDEFLAGS)  -o $@ $^ $(LIBFLAGS)

../libshapesOGL.o: ../libshapes.c
	$(MAKE) -C ..

../%.o: ../%.c
	$(MAKE) -C ..
clean:
	rm -f *.OVG *.OGL

indent:
	indent -linux -c 60 -brf -l 132 shapedemo.c hellovg.c mouse-hellovg.c particles.c screensize.c
