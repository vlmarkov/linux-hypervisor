all:
	make -C app
	make -C driver

clean:
	make -C app clean
	make -C driver clean
