TARGET = bfcallocator_benchmark

DYNAMIC_LIB = bfcallocator.so
STATIC_LIB = bfcallocator.a

COMPILE_FLAGS = -std=c++11 -Wall -Wextra -fPIC
DYNAMIC_LINK_FLAGS = -shared 
STATIC_LINK_FLAGS = -crv

HEADER_FILE = bfc_allocator.h
OBJ = bfc_allocator.o
MAIN_OBJ = test.o


CXX_COMPILER = g++

all:${DYNAMIC_LIB} ${STATIC_LIB} ${TARGET}
	@echo "make done, generate ${DYNAMIC_LIB} ${STATIC_LIB} ${TARGET}"


%.o:%.cc ${HEADER_FILE}
	${CXX_COMPILER} -c ${COMPILE_FLAGS} $< -o $@


${DYNAMIC_LIB}: ${OBJ}
	${CXX_COMPILER} ${DYNAMIC_LINK_FLAGS} $^ -o $@

${STATIC_LIB}: ${OBJ}
	ar ${STATIC_LINK_FLAGS}  $@ $^


${TARGET}: ${MAIN_OBJ} ${STATIC_LIB}

	${CXX_COMPILER} $< -o $@ -L. ${STATIC_LIB}

.PHONY:clean
clean:
	rm -rf ${TARGET}  ${STATIC_LIB} ${MAIN_OBJ} ${DYNAMIC_LIB} ${OBJ}