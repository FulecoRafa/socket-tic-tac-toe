.EXPORT_ALL_VARIABLES:
CC=gcc-10
C_FLAGS=-g -fsanitize=address
D_LIBS=-lpthread

run.server:
	make -C server run

run.client:
	make -C client run


build: build.server build.client

build.server:
	make -C server build

build.client:
	make -C client build

clean:
	make -C client clean
	make -C server clean
