ifeq ($(shell uname -s),Darwin)
SYSLIBS	=
C	= clang -fvisibility=hidden
CC	= clang++ -std=c++11 -fvisibility=hidden
else
SYSLIBS	= -Wl,-lpthread,-lstdc++
C	= gcc -fvisibility=hidden
CC	= g++ -std=gnu++11 -fvisibility=hidden
endif
APPCC	= ${CC} -g -c
APPLNK	= ${CC}
LIBCC	= ${APPCC} -fPIC
LIBLNK	= ${CC} -shared

all: libargo testargo argo

clean:
	rm -f libargo.so testargo argo test.* *.log *.o

install:
	cp libargo.so /usr/local/lib
	cp testargo /usr/local/bin
	cp argo /usr/local/bin

.PHONY:
	clean

LIBOBJS = channel.o driver.o json.o logger.o path.o socket.o udpchannel.o
TESTOBJS = testargo.o
APPOBJS = argo.o

libargo: ${LIBOBJS}
	${LIBLNK} ${LIBOBJS} ${SYSLIBS} -o $@.so

testargo: ${TESTOBJS}
	${APPLNK} ${TESTOBJS} ${SYSLIBS} libargo.so -o $@

argo: ${APPOBJS}
	${APPLNK} ${APPOBJS} ${SYSLIBS} libargo.so -o $@

argo.o: argo.cpp
	${APPCC} argo.cpp

channel.o: channel.cpp
	${LIBCC} channel.cpp

driver.o: driver.cpp
	${LIBCC} driver.cpp

json.o: json.cpp
	${LIBCC} json.cpp

logger.o: logger.cpp
	${LIBCC} logger.cpp

path.o: path.cpp
	${LIBCC} path.cpp

socket.o: socket.cpp
	${LIBCC} socket.cpp

testargo.o: testargo.cpp
	${APPCC} testargo.cpp

udpchannel.o: udpchannel.cpp
	${LIBCC} udpchannel.cpp

argo.cpp: idriver.h
	touch argo.cpp

channel.cpp: ascii.h channel.h
	touch channel.cpp

driver.cpp: channel.h driver.h logger.h
	touch driver.cpp

json.cpp: json.h
	touch json.cpp

logger.cpp: api.h logger.h
	touch log.cpp

path.cpp: path.h
	touch path.cpp

socket.cpp: logger.h socket.h
	touch socket.cpp

testargo.cpp: iapi.h
	touch testargo.cpp

udpchannel.cpp: udpchannel.h
	touch udpchannel.cpp

api.h: iapi.h
	touch api.h

channel.h: json.h socket.h
	touch channel.h

driver.h: api.h channel.h idriver.h json.h socket.h udpchannel.h
	touch driver.h

idriver.h: iapi.h
	touch idriver.h

json.h: api.h path.h
	touch json.h

logger.h:
	touch logger.h

path.h: api.h
	touch path.h

socket.h: api.h
	touch socket.h

udpchannel.h: channel.h
	touch udpchannel.h
