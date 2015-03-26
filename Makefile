all:asf

asf:
	#add path to shared lib aearch path
	#sudo cp ./lib/protobuf-c/libs/* /usr/local/lib/
	#sudo /sbin/ldconfig -v
	#begin to compile
	cd proto;./protoc-c --c_out=./ *.proto;make;cd -;
	cd src;make;cd -;
	cd test/client;make;cd -;

clean:
	cd proto;make clean;rm *.h *.c;cd -;
	cd src;make clean;cd -;
	cd test/client;make clean;cd -;

