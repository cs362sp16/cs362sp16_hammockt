CFLAGS = -std=c99 -Wall -fpic -coverage -lm

%.o: %.c %.h
	gcc -c $< -g $(CFLAGS)

dominion.o: dominion.h dominion.c rngs.o cardEffects.o dominion_helpers.o
	gcc -c dominion.c -g $(CFLAGS)

playdom: dominion.o playdom.c
	gcc -o playdom playdom.c -g dominion.o rngs.o cardEffects.o dominion_helpers.o $(CFLAGS)

player: player.c interface.o
	gcc -o player player.c -g  dominion.o rngs.o interface.o cardEffects.o dominion_helpers.o $(CFLAGS)

all: playdom player

clean:
	rm -f *.o playdom.exe playdom test.exe test player unittest1 unittest2 unittest3 unittest4 cardtest1 cardtest2 cardtest3 cardtest4 player.exe testInit testInit.exe *.gcov *.gcda *.gcno *.so *.a *.dSYM
