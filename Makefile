# Top level make file for truck-code

all:
	+ make -C include/utils
	+ make -C include/can
	+ make -C include/jbus
	+ make -C include/vision
	+ make -C src/
#	+ make -C tests/

install: all
	if [[ ! -d test ]] ; then mkdir -p test; fi
	+ make install -C include/utils
	+ make install -C include/jbus
	+ make install -C src/

clean:
	+ make clean -C include/utils
	+ make clean -C include/jbus
	+ make clean -C src/
