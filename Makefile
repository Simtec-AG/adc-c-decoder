# Makefile

#PATH	    := C:\MinGW\bin

CC	    := gcc
#ARCH	    := -m32
EXE	    := decode.exe

# ------------------------------------------------------------------------------

default: compile

# C-Compiler flags
#
CFLAGS      :=  -c -std=gnu99 ${ARCH} \
		-Wall ${SERIAL_DEBUG} \
		-O3 -g0 

# Linker flags (-s: strip)
LFLAGS      :=  -s

SOURCES	    := main.c adc_rs485_decoder.c serial.c print_msg.c

OBJECTS := ${SOURCES:.c=.o}
OBJECTS := ${OBJECTS:.S=.o}

%.o: %.c
	${CC} ${CFLAGS}  $< -o $@

%.o: %.S
	${CC} ${AFLAGS}  $< -o $@

${EXE}: ${OBJECTS}
	${CC} ${LFLAGS} ${OBJECTS} -o $@

# ------------------------------------------------------------------------------

compile: clean ${EXE}

# ------------------------------------------------------------------------------

.PHONY: clean
clean:
	del *.o

