FILES=${shell find . -name "*.c"}
BINS=${shell find dist -name "*.o"}

run:
	./dist/client.out

build: dist $(FILES)
	${CC} ${C_FLAGS} ${BINS} ${D_LIBS} -o dist/client.out
	@echo "Client build complete"

${FILES}: %.c: %
	echo $@

%:
	${CC} -c ${FLAGS} $*.c -o dist/$*.o

dist:
	@mkdir dist

clean:
	rm -rf dist
