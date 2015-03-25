FLAGS=-lprotobuf-c
INC=../../lib/protobuf-c/include/
LIB=../../lib/protbuf-c/libs/
BINS=client


all: ${BINS}

client: client.o ../../proto/cs_pkg.pb-c.o
	$(CC) -o $@ client.o ../../proto/cs_pkg.pb-c.o -I${INC} -L${LIB} ${FLAGS}
	@cp ./${BINS} ../../bin/
client.o: client.c
	$(CC) -c $^ -I${INC}

clean:
	/bin/rm -rf ${BINS} *.o core *.core
