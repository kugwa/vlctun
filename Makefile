all: dp
	

dp: dp.o vlctun.o
	gcc -o dp dp.o vlctun.o -pthread

dp.o: dp.c vlctun.h
	gcc -c -o dp.o dp.c

vlctun.o: vlctun.c
	gcc -c -o vlctun.o vlctun.c

clean:
	rm -f dp *.o
