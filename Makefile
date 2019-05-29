all:
	mkdir -p build
	make -C app
	make -C driver

clean:
	rm -Rf build
