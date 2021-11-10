TARGET = backtrace_benchmark



COMPILE_FLAGS = -g -O0 -std=c++14 -Wall -Wextra

HEADER_FILE = backtrace_service.h

MAIN_OBJ = backward_test.o


CXX_COMPILER = g++

CXX_LINKS = -lbfd -ldl

all:${TARGET} 
	@echo "make done, generate ${TARGET}"


%.o:example/%.cc ${HEADER_FILE}
	${CXX_COMPILER} -c ${COMPILE_FLAGS} $< -o $@


${TARGET}: ${MAIN_OBJ} 
	${CXX_COMPILER} $< -o $@ ${CXX_LINKS}

.PHONY:clean
clean:
	rm -rf ${TARGET} ${MAIN_OBJ} 