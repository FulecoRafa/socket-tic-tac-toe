CPP_COMPILER=g++
REPASS_VARS="CPP_COMPILER=${CPP_COMPILER}"

build.server:
	make -e -C server build ${REPASS_VARS}
