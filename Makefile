INSTALL_DIR := /usr/local/include


install:
	mkdir -p ${INSTALL_DIR}/bench
	cp bench.h ${INSTALL_DIR}/bench/

uninstall:
	rm -rf ${INSTALL_DIR}/uc

