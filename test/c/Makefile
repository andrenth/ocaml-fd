CFLAGS = -Wall

all:
	gcc ${CFLAGS} -c fd.c
	gcc ${CFLAGS} -c myopen.c
	gcc ${CFLAGS} -o mycat fd.o myopen.o mycat.c
	gcc ${CFLAGS} -o openfile fd.o openfile.c

clean:
	rm -f *.o
	rm -f mycat openfile
