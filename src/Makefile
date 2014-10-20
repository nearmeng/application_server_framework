CFLAGS=-Wall -g
LDFLAGS=-levent -lpthread
LIBS=workqueue.o
HEADERS=workqueue.h
BINS=server

INC=../lib/libevent-2.0.20-stable/include/
LIB=../lib/libevent-2.0.20-stable/.libs/

all: ${BINS}

server: server.o ${LIBS} ${HEADERS}
	${CC} server.o ${LIBS} -I${INC} -L${LIB} ${LDFLAGS} -o $@

clean:
	/bin/rm -rf ${BINS} *.o core *.core
