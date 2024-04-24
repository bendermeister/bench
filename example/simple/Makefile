FLAGS := -Wall -Wextra
DEBUG_FLAGS := ${FLAGS} -g3
RELEASE_FLAGS := ${FLAGS} -O3 -march=native -mtune=native

release: main.c
	${CC} ${RELEASE_FLAGS} main.c

debug: main.c
	${CC} ${DEBUG_FLAGS} main.c

clean:
	rm -f *.out
