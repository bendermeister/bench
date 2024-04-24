FLAGS := -Wall -Wextra
RELEASE_FLAGS := ${FLAGS} -march=native -mtune=native -O3
DEBUG_FLAGS := ${FLAGS} -g3
CC := clang

release: main.c
	${CC} ${RELEASE_FLAGS} main.c

debug: main.c
	${CC} ${DEBUG_FLAGS} main.c

clean:
	rm -f *.out
