all:asf

asf:
	cd proto;protoc-c --c_out=./ *.proto;make;cd -;
	cd src;make;cd -;
	cd test/client;make;cd -;

clean:
	cd proto;make clean;cd -;
	cd src;make clean;cd -;
	cd test/client;make clean;cd -;

