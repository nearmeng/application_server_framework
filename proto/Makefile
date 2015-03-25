all:cs_pkg.pb-c.o

CFLAGS=-Wall -g
LDFLAGS=-lpthread -levent -lprotobuf-c
INC=../lib/libevent-2.0.20-stable/include -I../lib/protobuf-c/include -I.
LIB=../lib/protobuf-c/libs -L../lib/libevent-2.0.20-stable/libs 

cs_pkg.pb-c.o:cs_pkg.pb-c.c
	${CC} ${CFLAGS} -c $^ -I${INC} -L${LIB} ${LDFLAGS} 
clean:
	/bin/rm -rf *.o *.core core
