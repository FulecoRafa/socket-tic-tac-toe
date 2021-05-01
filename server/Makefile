FILES=${shell find . -name "*.c"}
BINS=${shell find dist -name "*.o"}

run:
	./dist/server.out

build: dist $(FILES)
	${CC} -g ${C_FLAGS} ${BINS} ${D_LIBS} -o dist/server.out
	@echo "Server build complete"

${FILES}: %.c: %
	echo $@

%:
	${CC} -c ${FLAGS} $*.c -o dist/$*.o

dist:
	@mkdir dist

clean:
	rm -rf dist
