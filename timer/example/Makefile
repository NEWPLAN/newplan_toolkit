

CXX_FLAGS = -std=c++17 -Wall -Wextra 
HEADER_FILE = ../timeline.h ../time_record.h ../time_event.h
TIME_LINE_TARGET = timeline_benchmark
TIME_LINE_OBJ = timeline_benchmark.o
TIME_EVENT_TARGET = time_event_benchmark
TIME_EVENT_OBJ = time_event_benchmark.o
TIME_RECORD_TARGET = time_record_benchmark
TIME_RECORD_OBJ = time_record_benchmark.o

TARGET = ${TIME_LINE_TARGET} ${TIME_EVENT_TARGET} ${TIME_RECORD_TARGET}


CXX_COMPILER = g++

all: ${TARGET}
	@echo "make done, generate  ${TARGET}"


%.o:%.cc ${HEADER_FILE}
	${CXX_COMPILER} -c ${CXX_FLAGS} $< -o $@


${TIME_LINE_TARGET}: ${TIME_LINE_OBJ}
	${CXX_COMPILER} $< -o $@ 

${TIME_EVENT_TARGET}: ${TIME_EVENT_OBJ}
	${CXX_COMPILER} $< -o $@ 

${TIME_RECORD_TARGET}: ${TIME_RECORD_OBJ}
	${CXX_COMPILER} $< -o $@ 



.PHONY:clean
clean:
	rm -rf ${TARGET}  *.o 