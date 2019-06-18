# Top level make file for truck-code

all:
	+ make -C include/utils
	+ make -C include/can
	+ make -C include/jbus
	+ make -C include/logger
	+ make -C include/vision
	+ make -C src/
	+ make -C include/tests
	+ make -C tests/

install: all
	if [[ ! -d test ]] ; then mkdir -p test; fi
	+ make intstall -C include/utils
	+ make intstall -C include/can
	+ make intstall -C include/jbus
	+ make intstall -C include/logger
	+ make intstall -C include/vision
	+ make intstall -C src/

tests: all
	+ make -C tests/

clean:
	+ make clean -C include/utils
	+ make clean -C include/can
	+ make clean -C include/jbus
	+ make clean -C include/logger
	+ make clean -C include/vision
	+ make clean -C src/
	+ make clean -C tests/
