FLAGS=-lprotobuf-c
BINS=client

all: ${BINS}

client: client.o ../../proto/cs_pkg.pb-c.o
	${CC} -c client.c 
	$(CC) -o $@ client.o ../../proto/cs_pkg.pb-c.o ${FLAGS}
	@cp ./${BINS} ../../bin/


clean:
	/bin/rm -rf ${BINS} *.o core *.core
