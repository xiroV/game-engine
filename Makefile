ROOTDIR=$(shell pwd)
MODE=Release
BUILD_DIR=build_${MODE}

release:
	${MAKE} MODE=Release _build

debug:
	${MAKE} MODE=Debug _build

test:
	${MAKE} MODE=Debug _build_test

clean:
	${RM} -r -f build_*

_build_test: _prepare_build
	cmake --build ${ROOTDIR}/${BUILD_DIR} -- engine-test

_build: _prepare_build
	cmake --build ${ROOTDIR}/${BUILD_DIR} -- engine

_prepare_build:
	mkdir -p ${ROOTDIR}/${BUILD_DIR}
	cd ${ROOTDIR}/${BUILD_DIR} && cmake -DCMAKE_BUILD_TYPE=${MODE} ..


