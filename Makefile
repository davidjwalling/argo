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

uninstall:
	rm -f /usr/local/lib/libargo.so
	rm -f /usr/local/bin/argo
	rm -f /usr/local/bin/testargo

.PHONY:
	clean

LIBOBJS = aes.o api.o channel.o cipher.o des.o digest.o driver.o hmac.o json.o logger.o md5.o path.o pbc2.o prng.o random.o sha.o socket.o udpchannel.o
TESTOBJS = testargo.o
APPOBJS = argo.o

libargo: ${LIBOBJS}
	${LIBLNK} ${LIBOBJS} ${SYSLIBS} -o $@.so

testargo: ${TESTOBJS}
	${APPLNK} ${TESTOBJS} ${SYSLIBS} libargo.so -o $@

argo: ${APPOBJS}
	${APPLNK} ${APPOBJS} ${SYSLIBS} libargo.so -o $@

aes.o: aes.cpp
	${LIBCC} aes.cpp

api.o: api.cpp
	${LIBCC} api.cpp

argo.o: argo.cpp
	${APPCC} argo.cpp

channel.o: channel.cpp
	${LIBCC} channel.cpp

cipher.o: cipher.cpp
	${LIBCC} cipher.cpp

des.o: des.cpp
	${LIBCC} des.cpp

digest.o: digest.cpp
	${LIBCC} digest.cpp

driver.o: driver.cpp
	${LIBCC} driver.cpp

hmac.o: hmac.cpp
	${LIBCC} hmac.cpp

json.o: json.cpp
	${LIBCC} json.cpp

logger.o: logger.cpp
	${LIBCC} logger.cpp

md5.o: md5.cpp
	${LIBCC} md5.cpp

path.o: path.cpp
	${LIBCC} path.cpp

pbc2.o: pbc2.cpp
	${LIBCC} pbc2.cpp

prng.o: prng.cpp
	${LIBCC} prng.cpp

random.o: random.cpp
	${LIBCC} random.cpp

sha.o: sha.cpp
	${LIBCC} sha.cpp

socket.o: socket.cpp
	${LIBCC} socket.cpp

testargo.o: testargo.cpp
	${APPCC} testargo.cpp

udpchannel.o: udpchannel.cpp
	${LIBCC} udpchannel.cpp

aes.cpp: aes.h asn.h oid.h
	touch aes.cpp

api.cpp: api.h
	touch api.cpp

argo.cpp: idriver.h
	touch argo.cpp

channel.cpp: ascii.h channel.h
	touch channel.cpp

cipher.cpp: cipher.h random.h
	touch cipher.cpp

des.cpp: des.h asn.h oid.h
	touch des.cpp

digest.cpp: digest.h
	touch digest.cpp

driver.cpp: channel.h driver.h logger.h
	touch driver.cpp

hmac.cpp: hmac.h md5.h sha.h
	touch hmac.cpp

json.cpp: json.h
	touch json.cpp

logger.cpp: api.h logger.h
	touch logger.cpp

md5.cpp: md5.h asn.h oid.h
	touch md5.cpp

path.cpp: path.h
	touch path.cpp

pbc2.cpp: pbc2.h
	touch pbc2.cpp

prng.cpp: prng.h
	touch prng.cpp

random.cpp: random.h
	touch random.cpp

sha.cpp: sha.h asn.h oid.h
	touch sha.cpp

socket.cpp: logger.h socket.h
	touch socket.cpp

testargo.cpp: iapi.h
	touch testargo.cpp

udpchannel.cpp: udpchannel.h
	touch udpchannel.cpp

aes.h: api.h cipher.h
	touch aes.h

api.h: iapi.h
	touch api.h

asn.h: api.h
	touch asn.h

channel.h: json.h socket.h
	touch channel.h

cipher.h: api.h
	touch cipher.h

des.h: api.h cipher.h
	touch des.h

digest.h: api.h
	touch digest.h

driver.h: api.h channel.h idriver.h json.h socket.h udpchannel.h
	touch driver.h

hmac.h: api.h sha.h
	touch hmac.h

idriver.h: iapi.h
	touch idriver.h

json.h: api.h path.h
	touch json.h

logger.h:
	touch logger.h

md5.h: api.h digest.h
	touch md5.h

path.h: api.h
	touch path.h

pbc2.h: api.h hmac.h prng.h
	touch pbc2.h

prng.h: api.h des.h random.h
	touch prng.h

random.h: api.h
	touch random.h

sha.h: api.h digest.h
	touch sha.h

socket.h: api.h
	touch socket.h

udpchannel.h: channel.h
	touch udpchannel.h
