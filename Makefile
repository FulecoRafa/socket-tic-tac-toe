.EXPORT_ALL_VARIABLES:
CC=gcc
C_FLAGS=-g -fsanitize=address
D_LIBS=-lpthread -lcurses

run.server:
	make -C server run

run.client:
	make -C client run


build: build.server build.client

build.server:
	make -C server build

build.client:
	make -C client build

zip: clean
	zip socket-tic-tac-toe.zip -r . -x ".git/*" ".vscode/*" .gitignore "*/dist/*"

clean:
	make -C client clean
	make -C server clean
