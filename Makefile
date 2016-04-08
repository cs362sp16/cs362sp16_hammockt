CFLAGS = -Wall -fpic -coverage -lm

rngs.o: rngs.h rngs.c
	gcc -c rngs.c -g $(CFLAGS)

cardEffects.o: cardEffects.h cardEffects.c
	gcc -c cardEffects.c -g $(CFLAGS)

dominion.o: dominion.h dominion.c rngs.o cardEffects.o
	gcc -c dominion.c -g $(CFLAGS)

playdom: dominion.o playdom.c
	gcc -o playdom playdom.c -g dominion.o rngs.o cardEffects.o $(CFLAGS)

interface.o: interface.h interface.c
	gcc -c interface.c -g $(CFLAGS)

player: player.c interface.o
	gcc -o player player.c -g  dominion.o rngs.o interface.o cardEffects.o $(CFLAGS)

all: playdom player

clean:
	rm -f *.o playdom.exe playdom test.exe test player unittest1 unittest2 unittest3 unittest4 cardtest1 cardtest2 cardtest3 cardtest4 player.exe testInit testInit.exe *.gcov *.gcda *.gcno *.so *.a *.dSYM
