CFLAGS=-Wall -g
LDFLAGS=-levent -lprotobuf-c -lpthread 
LIBS=workqueue.o ../proto/cs_pkg.pb-c.o asf_op.o
HEADERS=workqueue.h ../proto/cs_pkg.pb-c.h
BINS=server

INC=../lib/libevent-2.0.20-stable/include/
LIB=../lib/libevent-2.0.20-stable/.libs/

all: ${BINS}

server: server.o ${LIBS} ${HEADERS}
	${CC} server.o ${LIBS} -I${INC} -L${LIB} ${LDFLAGS} -o $@
	@cp ./${BINS} ../bin/


clean:
	/bin/rm -rf ${BINS} *.o core *.core
