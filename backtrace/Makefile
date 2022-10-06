TARGET = backtrace_benchmark



COMPILE_FLAGS = -g -O0 -std=c++14 -Wall -Wextra

HEADER_FILE = backtrace_service.h

MAIN_OBJ = backward_test.o


CXX_COMPILER = g++

BFD_FLAGS_DETECTED = -lbfd

ifeq ($(OS),Windows_NT)
	@echo "detected on windows platform"
    # CCFLAGS += -D WIN32
    # ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
    #     CCFLAGS += -D AMD64
    # else
    #     ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
    #         CCFLAGS += -D AMD64
    #     endif
    #     ifeq ($(PROCESSOR_ARCHITECTURE),x86)
    #         CCFLAGS += -D IA32
    #     endif
    # endif
else
    UNAME_S := $(shell uname -s)
    ifeq (${UNAME_S},Linux)
        BFD_FLAGS_DETECTED = -lbfd
    endif
    ifeq ($(UNAME_S),Darwin)
        BFD_FLAGS_DETECTED = -lz
    endif
    # UNAME_P := $(shell uname -p)
    # ifeq ($(UNAME_P),x86_64)
    #     CCFLAGS += -D AMD64
    # endif
    # ifneq ($(filter %86,$(UNAME_P)),)
    #     CCFLAGS += -D IA32
    # endif
    # ifneq ($(filter arm%,$(UNAME_P)),)
    #     CCFLAGS += -D ARM
    # endif
endif

CXX_LINKS = ${BFD_FLAGS_DETECTED} -ldl -lpthread

all:${TARGET} 
	@echo "make done, generate ${TARGET}"


%.o:example/%.cc ${HEADER_FILE}
	${CXX_COMPILER} -c ${COMPILE_FLAGS} $< -o $@


${TARGET}: ${MAIN_OBJ} 
	${CXX_COMPILER} $< -o $@ ${CXX_LINKS}

.PHONY:clean
clean:
	rm -rf ${TARGET} ${MAIN_OBJ} 