CFLAGS=-Wall -g
LDFLAGS=-lpthread -levent -lprotobuf-c
INC=../lib/libevent-2.0.20-stable/include/ -I../lib/protobuf-c/include/ -I.
LIB=../lib/protobuf-c/libs/ -L../lib/libevent-2.0.20-stable/libs/

BINS=server

all: ${BINS}

server: workqueue.o asf_op.o server.o
	${CC} $^ ../proto/cs_pkg.pb-c.o  -I${INC} -L${LIB} ${LDFLAGS} -o $@
	@cp ./${BINS} ../bin/

server.o: server.c
	${CC} ${CFLAGS} -c $^  -I${INC} -L${LIB} ${LDFLAGS}

workqueue.o:workqueue.c
	${CC} ${CFLAGS} -c $^ -lpthread


asf_op.o:asf_op.c
	${CC} ${CFLAGS} -c $^ -I${INC} -L${LIB} ${LDFLAGS}

clean:
	/bin/rm -rf ${BINS} *.o core *.core
	/bin/rm -rf ../bin/server
