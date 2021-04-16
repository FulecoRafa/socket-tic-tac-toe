FILES=${shell find . -name "*.c" -printf "%f "}
BINS=${shell find dist -name "*.o"}

run:
	./dist/server.out

build: dist $(FILES)
	${CC} ${C_FLAGS} ${BINS} ${D_LIBS} -o dist/server.out
	@echo "Server build complete"

${FILES}: %.c: %
	echo $@

%:
	${CC} -c ${FLAGS} $*.c -o dist/$*.o

dist:
	@mkdir dist

clean:
	rm -rf dist
