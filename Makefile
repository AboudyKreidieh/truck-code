# Top level make file for Truck code

all:
	+ make -C src/utils
	+ make -C src/jbus

install: all
	if [[ ! -d test ]] ; then mkdir -p test; fi
	+ make install -C src/utils
	+ make install -C src/jbus

clean:
	+ make clean -C src/utils
	+ make clean -C src/jbus
