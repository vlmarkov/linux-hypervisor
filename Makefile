all: driver app

app:
	make -C hypervisor2 all

driver:
	make -C lkm all

clean:
	make -C hypervisor2 clean
	make -C lkm clean
