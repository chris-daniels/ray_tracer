# Makefile 
#Raytracer Project

# assume the pic directory locates one level above,
# change PIC_PATH if this is not the case
PIC_PATH = $(abspath $(CURDIR)/../pic)

INCLUDE = -I$(PIC_PATH)
LIBRARIES = -L$(PIC_PATH) -framework OpenGL -framework GLUT -lpicio $(PIC_PATH)/libjpeg.a


COMPILER = g++
COMPILERFLAGS = -O3 $(INCLUDE)

PROGRAM = raytracer
SOURCE = raytracer.cpp
OBJECT = raytracer.o

.cpp.o: 
	$(COMPILER) -c $(COMPILERFLAGS) $<

all: $(PROGRAM)

$(PROGRAM): $(OBJECT)
	$(COMPILER) $(COMPILERFLAGS) -o $(PROGRAM) $(OBJECT) $(LIBRARIES)

clean:
	-rm -rf core *.o *~ "#"*"#" $(PROGRAM)